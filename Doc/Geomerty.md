

##### Triangulation

将一个Poly三角化，如有n个顶点：

- 在0到n-1之间找到一个spilt，split是最佳划分。可以得到一个三角形。
- 然后对子区间按照同样的方法递归划分，最终将Poly三角化。
- 计算最佳划分split时，会为这个三角形计算一个代价（可以是面积的平方），加上左右区间的代价。通过最小化总代叫确定split

$$
[0,n-1]
$$

![image-20240820135812128](./Geomerty.assets/image-20240820135812128.png)

具体实现是利动态规划做预计算，算出每个区间对应的最佳划分split。

伪代码

```c++
void Triangulation::triangulate(Face f)
{
    // collect polygon halfedges
    Halfedge h0 = mesh_.halfedge(f);
    halfedges_.clear();
    vertices_.clear();
    Halfedge h = h0;
    do
    {
        if (!mesh_.is_manifold(mesh_.to_vertex(h)))
        {
            auto what = std::string{ __func__ } + ": Non-manifold polygon";
            throw InvalidInputException(what);
        }

        halfedges_.emplace_back(h);
        vertices_.emplace_back(mesh_.to_vertex(h));
    } while ((h = mesh_.next_halfedge(h)) != h0);

    // do we have at least four vertices?
    const auto n = halfedges_.size();
    if (n <= 3)
        return;

    // compute minimal triangulation by dynamic programming
    weight_.clear();
    weight_.resize(n,
        std::vector<Scalar>(n, std::numeric_limits<Scalar>::max()));
    index_.clear();
    index_.resize(n, std::vector<int>(n, 0));

    // initialize 2-gons
    for (size_t i = 0; i < n - 1; ++i)
    {
        weight_[i][i + 1] = 0.0;
        index_[i][i + 1] = -1;
    }

    // n-gons with n>2
    for (size_t j = 2; j < n; ++j)
    {
        // for all n-gons [i,i+j]
        for (size_t i = 0; i < n - j; ++i)
        {
            auto k = i + j;
            auto wmin = std::numeric_limits<Scalar>::max();
            auto imin = -1;

            // find best split i < m < i+j
            for (size_t m = i + 1; m < k; ++m)
            {
                Scalar w =
                    weight_[i][m] + compute_weight(i, m, k) + weight_[m][k];

                if (w < wmin)
                {
                    wmin = w;
                    imin = m;
                }
            }

            weight_[i][k] = wmin;
            index_[i][k] = imin;
        }
    }

    // now add triangles to mesh
    std::vector<ivec2> todo;
    todo.reserve(n);
    todo.emplace_back(0, n - 1);
    while (!todo.empty())
    {
        ivec2 tri = todo.back();
        todo.pop_back();
        int start = tri[0];
        int end = tri[1];
        if (end - start < 2)
            continue;
        int split = index_[start][end];

        insert_edge(start, split);
        insert_edge(split, end);

        todo.emplace_back(start, split);
        todo.emplace_back(split, end);
    }

    // clean up
    weight_.clear();
    index_.clear();
    halfedges_.clear();
    vertices_.clear();
}

```

##### Smooth

算mesh拉普拉斯算子，对mesh坐标进行偏移，达到光滑的效果。

分为explicit_smoothing和implicit_smoothing两种。

在explicit_smoothing中：利用laplace矩阵得到偏移量，然后偏移坐标，可迭代多次。

![image-20240820235550333](Geomerty.assets/image-20240820235550333.png)

在implicit_smoothing中：通过构造smooth后坐标矩阵的隐式方程，然后求解线性系统，得到smooth结果

![image-20240820235904097](Geomerty.assets/image-20240820235904097.png)



#### Nurbs

##### average_knot_vector

给出一堆离散点（通常遍布了nurbs curve）的参数，根据阶数生成，采用平均的方式生成节向量。

- 首尾degree+1个节元素为0或1
- 中间的节元素，为degree长度的区间上参数求平均。

![image-20240821165112733](./Geomerty.assets/image-20240821165112733.png)

##### bspline_one_basis

B样条基函数的计算：

![img](./Geomerty.assets/1d0804b98238e599b281f4b42fc53417.jpeg)

采用递归的方式计算复杂度较高，可采用迭代的方式计算,在迭代方式计算中采用动态规划的方式。

1. 先把第0阶的基函数算出来，然后也一直向前递推到计算到d阶基函数。
2. 最开始需要i到i+1+d对应顶点的0阶基函数
3. 然后高阶利用低阶的结果计算

![image-20240821184057887](./Geomerty.assets/image-20240821184057887.png)

```c++
inline scalar bspline_one_basis(int32 i, size_t deg, const Array<scalar>& U, scalar u)
{
    int32 m = static_cast<int32>(U.size()) - 1;
    // Special case
    if((i == 0 && close(u, U[0])) || (i == m - deg - 1 && close(u, U[m])))
    {
        return 1.0;
    }
    // Local property ensures that basis function is zero outside span
    if(u < U[i] || u >= U[i + deg + 1])
    {
        return 0.0;
    }
    // Initialize zeroth-degree functions
    Array<scalar> N;
    N.resize(deg + 1);
    for(auto j: IndexRange(deg + 1))
    {
        N[j] = (u >= U[i + j] && u < U[i + j + 1]) ? 1.0 : 0.0;
    }
    // Compute triangular table
    for(auto k: IndexRange<size_t>(1, deg + 1))
    {
        scalar saved = (close(N[0], 0.0)) ? 0.0 : ((u - U[i]) * N[0]) / (U[i + k] - U[i]);
        for(auto j: IndexRange(deg - k + 1))
        {
            scalar Uleft = U[i + j + 1];
            scalar Uright = U[i + j + k + 1];
            if(close(N[j + 1], 0.0))
            {
                N[j] = saved;
                saved = 0.0;
            }
            else
            {
                scalar temp = N[j + 1] / (Uright - Uleft);
                N[j] = saved + (Uright - u) * temp;
                saved = (u - Uleft) * temp;
            }
        }
    }
    return N[0];
}
```

