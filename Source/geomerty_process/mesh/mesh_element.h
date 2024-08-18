#pragma once
#include<iostream>
#include<assert.h>

namespace Geomerty
{ namespace Mesh {
inline constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();
// mesh element
class Point;
class Vertex;
class Prim;
struct Edge;
struct HalfEdge;
enum class PrimType :uint8_t
{
    Polygon = 0,
    Polyline,
    Instance,
    VDB,
    NurbsCurve,
    NurbsSurface
};
class Point
{
public:
    Point();
    size_t vertex() const noexcept;
    const Vertex* vertex_ptr() const noexcept { return m_vertex; };
    Vertex* vertex_ptr() noexcept { return m_vertex; };
    size_t idx() const noexcept { return m_index; }

 
    void set_vertex(Vertex* vertex) noexcept { m_vertex = vertex; }
    void add_vertex(Vertex* vertex) noexcept;
    void remove_vertex(Vertex* vertex) noexcept;

private:
    void reset() noexcept;
    void set_idx(size_t new_idx) noexcept { m_index = new_idx; };

    Vertex* m_vertex;
    size_t m_index;
};

class Vertex
{
    friend Point;
    friend Prim;
    //friend VertexContainer;
  
public:
    Vertex();
    size_t prim() const noexcept;
    size_t point() const noexcept
    {
        assert(m_point);
        return m_point->idx();
    }
    size_t pre_vertex() const noexcept { return m_pre_vertex ? m_pre_vertex->idx() : INVALID_INDEX; };
    size_t next_vertex() const noexcept { return m_next_vertex ? m_next_vertex->idx() : INVALID_INDEX; };
    size_t pre_point_vertex() const noexcept { return m_pre_point_vertex ? m_pre_point_vertex->idx() : INVALID_INDEX; };
    size_t next_point_vertex() const noexcept
    {
        return m_next_point_vertex ? m_next_point_vertex->idx() : INVALID_INDEX;
    };
    const Prim* prim_ptr() const noexcept { return m_prim; }
    const Point* point_ptr() const noexcept { return m_point; }
    const Vertex* pre_vertex_ptr() const noexcept { return m_pre_vertex; };
    const Vertex* next_vertex_ptr() const noexcept { return m_next_vertex; };
    const Vertex* pre_point_vertex_ptr() const noexcept { return m_pre_point_vertex; };
    const Vertex* next_point_vertex_ptr() const noexcept { return m_next_point_vertex; };

    Prim* prim_ptr() noexcept { return m_prim; }
    Point* point_ptr() noexcept { return m_point; }
    Vertex* pre_vertex_ptr() noexcept { return m_pre_vertex; };
    Vertex* next_vertex_ptr() noexcept { return m_next_vertex; };
    Vertex* pre_point_vertex_ptr() noexcept { return m_pre_point_vertex; };
    Vertex* next_point_vertex_ptr() noexcept { return m_next_point_vertex; };

    bool is_valid() const noexcept { return m_point != nullptr && m_prim != nullptr; }
    size_t idx() const noexcept { return m_index; }


    void set_prim(Prim* prim) noexcept { m_prim = prim; }
    void set_point(Point* point) noexcept { m_point = point; }
    void set_pre_vertex(Vertex* vertex) noexcept { m_pre_vertex = vertex; };
    void set_next_vertex(Vertex* vertex) noexcept { m_next_vertex = vertex; };
    void set_pre_point_vertex(Vertex* vertex) noexcept { m_pre_point_vertex = vertex; };
    void set_next_point_vertex(Vertex* vertex) noexcept { m_next_point_vertex = vertex; };

private:
    void reset() noexcept;

    void set_idx(size_t new_idx) noexcept { m_index = new_idx; };

    Vertex* m_pre_vertex;
    Vertex* m_next_vertex;

    Vertex* m_pre_point_vertex;
    Vertex* m_next_point_vertex;

    Prim* m_prim;
    Point* m_point;

    size_t m_index;
};

class  Prim
{
   // friend PrimContainer;
   // N_NON_COPYABLE(Prim)

public:
    explicit Prim(PrimType type = PrimType::Polygon);

    bool is_valid() const noexcept { return m_vertex != nullptr; }
    size_t idx() const noexcept { return m_index; }
    PrimType type() const noexcept { return m_type; }
    size_t vertex() const noexcept
    {
        assert(m_vertex);
        return m_vertex->idx();
    }
    Vertex* vertex_ptr() noexcept { return m_vertex; }
    const Vertex* vertex_ptr() const noexcept { return m_vertex; }
    size_t num_vertices() const noexcept { return m_num_vertices; }
    bool is_closed() const noexcept { return m_vertex && m_vertex->pre_vertex_ptr(); }


    void set_vertex(Vertex* vertex) noexcept { m_vertex = vertex; }
    void set_type(PrimType type) noexcept { m_type = type; }
    void set_num_vertices(size_t num_vertices) noexcept { m_num_vertices = num_vertices; }
    void remove_vertex(Vertex* vertex);
    void increase_num_vertices() noexcept { m_num_vertices++; }
    void decrease_num_vertices() noexcept { m_num_vertices--; }

private:
    void reset() noexcept;
    void set_idx(size_t new_idx) noexcept { m_index = new_idx; };

    Vertex* m_vertex;
    size_t m_num_vertices;
    PrimType m_type;
    size_t m_index;
};

size_t Point::vertex() const noexcept { return m_vertex ? m_vertex->idx() : INVALID_INDEX; };
size_t Vertex::prim() const noexcept { return m_prim ? m_prim->idx() : INVALID_INDEX; }

struct HalfEdge
{
    size_t start;
    size_t end;
};


struct Edge
{
    size_t point0;
    size_t point1;

    bool operator==(const Edge& lhs) const noexcept
    {
        return (lhs.point0 == point0 && lhs.point1 == point1) || (lhs.point0 == point1 && lhs.point1 == point0);
    }

     bool operator!=(const Edge& lhs) const noexcept
    {
        return (lhs.point0 != point0 || lhs.point1 != point1) && (lhs.point0 != point1 || lhs.point1 != point0);
    }
};
    }


}// namespace nous
