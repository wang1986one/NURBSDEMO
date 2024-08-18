#pragma once
#include"algorithm/smooth.h"
#include <Eigen/Sparse>
#include <Eigen/Dense>

namespace Geomerty {

using Triplet = Eigen::Triplet<double>;

void compute_virtual_vertex(const Eigen::MatrixXd& poly, Eigen::VectorXd& weights)
{
    const int n = poly.rows();

    // setup array of positions and edges
    std::vector<dvec3> x(n), d(n);
    for (int i = 0; i < n; ++i)
        x[i] = poly.row(i);
    for (int i = 0; i < n; ++i)
        d[i] = x[(i + 1) % n] - x[i];

    // setup matrix A and rhs b
    // see Equation (38) of "Polygon Laplacian made simple", Eurographics 2020
    Eigen::MatrixXd A(n + 1, n);
    Eigen::VectorXd b(n + 1);
    for (int j = 0; j < n; ++j)
    {
        for (int i = j; i < n; ++i)
        {
            double Aij(0.0), bi(0.0);
            for (int k = 0; k < n; ++k)
            {
                Aij += dot(cross(x[j], d[k]), cross(x[i], d[k]));
                bi += dot(cross(x[i], d[k]), cross(x[k], d[k]));
            }
            A(i, j) = A(j, i) = Aij;
            b(i) = bi;
        }
    }
    for (int j = 0; j < n; ++j)
    {
        A(n, j) = 1.0;
    }
    b(n) = 1.0;

    weights = A.completeOrthogonalDecomposition().solve(b).topRows(n);
}

void triangle_laplace_matrix(const Eigen::Vector3d& p0,
    const Eigen::Vector3d& p1,
    const Eigen::Vector3d& p2, Eigen::MatrixXd& Ltri)
{
    std::array<double, 3> l, l2, cot;

    // squared edge lengths
    l2[0] = (p1 - p2).squaredNorm();
    l2[1] = (p0 - p2).squaredNorm();
    l2[2] = (p0 - p1).squaredNorm();

    // edge lengths
    l[0] = sqrt(l2[0]);
    l[1] = sqrt(l2[1]);
    l[2] = sqrt(l2[2]);

    // triangle area
    const double arg = (l[0] + (l[1] + l[2])) * (l[2] - (l[0] - l[1])) *
        (l[2] + (l[0] - l[1])) * (l[0] + (l[1] - l[2]));
    const double area = 0.5 * sqrt(arg);

    if (area > std::numeric_limits<double>::min())
    {
        // cotangents of angles
        cot[0] = 0.25 * (l2[1] + l2[2] - l2[0]) / area;
        cot[1] = 0.25 * (l2[2] + l2[0] - l2[1]) / area;
        cot[2] = 0.25 * (l2[0] + l2[1] - l2[2]) / area;

        Ltri(0, 0) = cot[1] + cot[2];
        Ltri(1, 1) = cot[0] + cot[2];
        Ltri(2, 2) = cot[0] + cot[1];
        Ltri(1, 0) = Ltri(0, 1) = -cot[2];
        Ltri(2, 0) = Ltri(0, 2) = -cot[1];
        Ltri(2, 1) = Ltri(1, 2) = -cot[0];
    }
}
void polygon_laplace_matrix(const Eigen::MatrixXd& polygon, Eigen::MatrixXd& Lpoly)
{
    const int n = (int)polygon.rows();
    Lpoly = Eigen::MatrixXd::Zero(n, n);

    // shortcut for triangles
    if (n == 3)
    {
        triangle_laplace_matrix(polygon.row(0), polygon.row(1), polygon.row(2),
            Lpoly);
        return;
    }

    // compute position of virtual vertex
    Eigen::VectorXd vweights;
    compute_virtual_vertex(polygon, vweights);
    Eigen::Vector3d vvertex = polygon.transpose() * vweights;

    // laplace matrix of refined triangle fan
    Eigen::MatrixXd Lfan = Eigen::MatrixXd::Zero(n + 1, n + 1);
    Eigen::MatrixXd Ltri(3, 3);
    for (int i = 0; i < n; ++i)
    {
        const int j = (i + 1) % n;

        // build laplace matrix of one triangle
        triangle_laplace_matrix(polygon.row(i), polygon.row(j), vvertex, Ltri);

        // assemble to laplace matrix for refined triangle fan
        Lfan(i, i) += Ltri(0, 0);
        Lfan(i, j) += Ltri(0, 1);
        Lfan(i, n) += Ltri(0, 2);
        Lfan(j, i) += Ltri(1, 0);
        Lfan(j, j) += Ltri(1, 1);
        Lfan(j, n) += Ltri(1, 2);
        Lfan(n, i) += Ltri(2, 0);
        Lfan(n, j) += Ltri(2, 1);
        Lfan(n, n) += Ltri(2, 2);
    }

    // build prolongation matrix
    Eigen::MatrixXd P(n + 1, n);
    P.setIdentity();
    P.row(n) = vweights;

    // build polygon laplace matrix by sandwiching
    Lpoly = P.transpose() * Lfan * P;
}
void uniform_laplace_matrix(const SurfaceMesh& mesh, Eigen::SparseMatrix<double>& L)
{
    const unsigned int n = mesh.n_vertices();

    std::vector<Triplet> triplets;
    triplets.reserve(8 * n); // conservative estimate for triangle meshes

    for (auto vi : mesh.vertices())
    {
        Scalar sum_weights = 0.0;
        for (auto vj : mesh.vertices(vi))
        {
            sum_weights += 1.0;
            triplets.emplace_back(vi.idx(), vj.idx(), 1.0);
        }
        triplets.emplace_back(vi.idx(), vi.idx(), -sum_weights);
    }

    L.resize(n, n);
    L.setFromTriplets(triplets.begin(), triplets.end());
}
void laplace_matrix(const SurfaceMesh& mesh, Eigen::SparseMatrix<double>& L, bool clamp)
{
    const int nv = mesh.n_vertices();
    const int nf = mesh.n_faces();
    std::vector<Vertex> vertices; // polygon vertices
    Eigen::MatrixXd polygon;          // positions of polygon vertices
    Eigen::MatrixXd Lpoly;            // local laplace matrix

    std::vector<Triplet> triplets;
    triplets.reserve(9 * nf); // estimate for triangle meshes

    for (Face f : mesh.faces())
    {
        // collect polygon vertices
        vertices.clear();
        for (Vertex v : mesh.vertices(f))
        {
            vertices.push_back(v);
        }
        const int n = vertices.size();

        // collect their positions
        polygon.resize(n, 3);
        for (int i = 0; i < n; ++i)
        {
            polygon.row(i) = (Eigen::Vector3d)mesh.position(vertices[i]);
        }

        // setup local laplace matrix
        polygon_laplace_matrix(polygon, Lpoly);

        // assemble to global laplace matrix
        for (int j = 0; j < n; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                triplets.emplace_back(vertices[k].idx(), vertices[j].idx(),
                    -Lpoly(k, j));
            }
        }
    }

    // build sparse matrix from triplets
    L.resize(nv, nv);
    L.setFromTriplets(triplets.begin(), triplets.end());

    // clamp negative off-diagonal entries to zero
    if (clamp)
    {
        for (unsigned int k = 0; k < L.outerSize(); k++)
        {
            double diag_offset(0.0);

            for (Eigen::SparseMatrix<double>::InnerIterator iter(L, k); iter; ++iter)
            {
                if (iter.row() != iter.col() && iter.value() < 0.0)
                {
                    diag_offset += -iter.value();
                    iter.valueRef() = 0.0;
                }
            }
            for (Eigen::SparseMatrix<double>::InnerIterator iter(L, k); iter; ++iter)
            {
                if (iter.row() == iter.col() && iter.value() < 0.0)
                    iter.valueRef() -= diag_offset;
            }
        }
    }
}
void triangle_gradient_matrix(const Eigen::Vector3d& p0,
    const Eigen::Vector3d& p1,
    const Eigen::Vector3d& p2, Eigen::MatrixXd& G)
{
    G.resize(3, 3);
    Eigen::Vector3d n = (p1 - p0).cross(p2 - p0);
    n /= n.squaredNorm();
    G.col(0) = n.cross(p2 - p1);
    G.col(1) = n.cross(p0 - p2);
    G.col(2) = n.cross(p1 - p0);
}

void polygon_gradient_matrix(const Eigen::MatrixXd& polygon, Eigen::MatrixXd& Gpoly)
{
    const int n = (int)polygon.rows();

    // compute position of virtual vertex
    Eigen::VectorXd vweights;
    compute_virtual_vertex(polygon, vweights);
    Eigen::Vector3d vvertex = polygon.transpose() * vweights;

    Eigen::MatrixXd Gfan = Eigen::MatrixXd::Zero(3 * n, n + 1);
    Eigen::MatrixXd Gtri(3, 3);
    int row = 0;
    for (int i = 0; i < n; ++i)
    {
        const int j = (i + 1) % n;

        // build laplace matrix of one triangle
        triangle_gradient_matrix(polygon.row(i), polygon.row(j), vvertex, Gtri);

        // assemble to matrix for triangle fan
        for (int k = 0; k < 3; ++k)
        {
            Gfan(row + k, i) += Gtri(k, 0);
            Gfan(row + k, j) += Gtri(k, 1);
            Gfan(row + k, n) += Gtri(k, 2);
        }

        row += 3;
    }

    // build prolongation matrix
    Eigen::MatrixXd P(n + 1, n);
    P.setIdentity();
    P.row(n) = vweights;

    // build polygon gradient matrix by sandwiching (from left only)
    Gpoly = Gfan * P;
}
void gradient_matrix(const SurfaceMesh& mesh, Eigen::SparseMatrix<double>& G)
{
    const int nv = mesh.n_vertices();

    // how many virtual triangles will we have after refinement?
    // triangles are not refined, other polygons are.
    unsigned int nt = 0;
    for (auto f : mesh.faces())
    {
        nt += mesh.valence(f);
    }

    std::vector<Vertex> vertices; // polygon vertices
    Eigen::MatrixXd polygon;          // positions of polygon vertices
    Eigen::MatrixXd Gpoly;            // local gradient matrix

    std::vector<Triplet> triplets;
    triplets.reserve(9 * nt);

    unsigned int n_rows = 0;

    for (Face f : mesh.faces())
    {
        // collect polygon vertices
        vertices.clear();
        for (Vertex v : mesh.vertices(f))
        {
            vertices.push_back(v);
        }
        const int n = vertices.size();

        // collect their positions
        polygon.resize(n, 3);
        for (int i = 0; i < n; ++i)
        {
            polygon.row(i) = (Eigen::Vector3d)mesh.position(vertices[i]);
        }

        // setup local element matrix
        polygon_gradient_matrix(polygon, Gpoly);

        // assemble to global matrix
        for (int j = 0; j < Gpoly.cols(); ++j)
        {
            for (int i = 0; i < Gpoly.rows(); ++i)
            {
                triplets.emplace_back(n_rows + i, vertices[j].idx(),
                    Gpoly(i, j));
            }
        }

        n_rows += Gpoly.rows();
    }
    assert(n_rows == 3 * nt);

    // build sparse matrix from triplets
    G.resize(n_rows, nv);
    G.setFromTriplets(triplets.begin(), triplets.end());
}
double triarea(const Eigen::Vector3d& p0, const Eigen::Vector3d& p1,
    const Eigen::Vector3d& p2)
{
    return 0.5 * (p1 - p0).cross(p2 - p0).norm();
}
void divmass_matrix(const SurfaceMesh& mesh, Eigen::DiagonalMatrix<double, Eigen::Dynamic>& M)
{
    // how many virtual triangles will we have after refinement?
    unsigned int nt = 0;
    for (auto f : mesh.faces())
    {
        nt += mesh.valence(f);
    }

    // initialize global matrix
    M.resize(3 * nt);
    auto& diag = M.diagonal();

    std::vector<Vertex> vertices; // polygon vertices
    Eigen::MatrixXd polygon;          // positions of polygon vertices

    unsigned int idx = 0;

    for (Face f : mesh.faces())
    {
        // collect polygon vertices
        vertices.clear();
        for (Vertex v : mesh.vertices(f))
        {
            vertices.push_back(v);
        }
        const int n = vertices.size();

        // collect their positions
        polygon.resize(n, 3);
        for (int i = 0; i < n; ++i)
        {
            polygon.row(i) = (Eigen::Vector3d)mesh.position(vertices[i]);
        }

        // compute position of virtual vertex
        Eigen::VectorXd vweights;
        compute_virtual_vertex(polygon, vweights);
        Eigen::Vector3d vvertex = polygon.transpose() * vweights;

        for (int i = 0; i < n; ++i)
        {
            const double area =
                triarea(polygon.row(i), polygon.row((i + 1) % n), vvertex);

            diag[idx++] = area;
            diag[idx++] = area;
            diag[idx++] = area;
        }
    }

    assert(idx == 3 * nt);
}
void divergence_matrix(const SurfaceMesh& mesh, Eigen::SparseMatrix<double>& D)
{
    Eigen::SparseMatrix<double> G;
    gradient_matrix(mesh, G);
    Eigen::DiagonalMatrix<double, Eigen::Dynamic> M;
    divmass_matrix(mesh, M);
    D = -G.transpose() * M;
}
void selector_matrix(const SurfaceMesh& mesh,
    const std::function<bool(Vertex)>& is_selected,
    Eigen::SparseMatrix<double>& S)
{
    std::vector<Triplet> triplets;
    triplets.reserve(mesh.n_vertices());

    int row = 0;
    for (auto v : mesh.vertices())
    {
        if (is_selected(v))
        {
            triplets.emplace_back(row++, v.idx(), 1.0);
        }
    }

    S.resize(row, mesh.n_vertices());
    S.setFromTriplets(triplets.begin(), triplets.end());
}
void coordinates_to_matrix(const SurfaceMesh& mesh, Eigen::MatrixXd& X)
{
    X.resize(mesh.n_vertices(), 3);
    for (auto v : mesh.vertices())
        X.row(v.idx()) = static_cast<Eigen::Vector3d>(mesh.position(v));
}
void matrix_to_coordinates(const Eigen::MatrixXd& X, SurfaceMesh& mesh)
{
    assert((size_t)X.rows() == mesh.n_vertices() && X.cols() == 3);
    for (auto v : mesh.vertices())
        mesh.position(v) = X.row(v.idx());
}
void explicit_smoothing(SurfaceMesh& mesh, unsigned int iterations, bool use_uniform_laplace)
{
    if (!mesh.n_vertices())
        return;

    // Laplace matrix (clamp negative cotan weights to zero)
    Eigen::SparseMatrix<double> L;
    if (use_uniform_laplace)
        uniform_laplace_matrix(mesh, L);
    else
        laplace_matrix(mesh, L, true);

    Eigen::SparseMatrix<double> G, D, L1, L2;
    laplace_matrix(mesh, L1,false);
    gradient_matrix(mesh, G);
    divergence_matrix(mesh, D);
    L2 = D * G;

    // normalize each row by sum of weights
    // scale by 0.5 to make it more robust
    // multiply by -1 to make it neg. definite again
    L = -0.5 * L.diagonal().asDiagonal().inverse() * L;

    // cancel out boundary vertices
    Eigen::SparseMatrix<double> S;
    auto is_inner = [&](Vertex v) { return !mesh.is_boundary(v); };
    selector_matrix(mesh, is_inner, S);
    L = S.transpose() * S * L;

    // copy vertex coordinates to matrix
    Eigen::MatrixXd X;
    coordinates_to_matrix(mesh, X);

    // perform some iterations
    for (unsigned int i = 0; i < iterations; ++i)
        X += L * X;

    // copy matrix back to vertex coordinates
    matrix_to_coordinates(X, mesh);
}

void implicit_smoothing(SurfaceMesh& mesh, Scalar timestep, unsigned int iterations, bool use_uniform_laplace, bool rescale)
{
}

}