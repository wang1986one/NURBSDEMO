#include "mesh_element.h"

namespace Geomerty
{
    namespace Mesh {
Point::Point() : m_vertex(nullptr), m_index(INVALID_INDEX) {}

void Point::remove_vertex(Vertex* vertex) noexcept
{
    assert(vertex->point_ptr() == this);
    vertex->m_point = nullptr;
    if(vertex == m_vertex)
    {
        m_vertex = m_vertex->next_point_vertex_ptr();
        if(m_vertex)
        {
            m_vertex->m_pre_point_vertex = nullptr;
            vertex->m_next_point_vertex = nullptr;
        }
    }
    else
    {
        if(vertex->pre_point_vertex_ptr())
        {
            vertex->pre_point_vertex_ptr()->m_next_point_vertex = vertex->next_point_vertex_ptr();
        }
        if(vertex->next_point_vertex_ptr())
        {
            vertex->next_point_vertex_ptr()->m_pre_point_vertex = vertex->pre_point_vertex_ptr();
        }
        vertex->m_pre_point_vertex = nullptr;
        vertex->m_next_point_vertex = nullptr;
    }
}

void Point::add_vertex(Vertex* vertex) noexcept
{
    vertex->m_next_point_vertex = m_vertex;
    vertex->m_pre_point_vertex = nullptr;
    if(m_vertex)
    {
        m_vertex->m_pre_point_vertex = vertex;
    }
    m_vertex = vertex;
    m_vertex->m_point = this;
}

void Point::reset() noexcept
{
    m_vertex = nullptr;
    m_index = INVALID_INDEX;
}

Vertex::Vertex()
    : m_pre_vertex(nullptr), m_next_vertex(nullptr), m_pre_point_vertex(nullptr), m_next_point_vertex(nullptr), m_prim(nullptr), m_point(nullptr),
      m_index(INVALID_INDEX)
{
}

void Vertex::reset() noexcept
{
    m_pre_vertex = nullptr;
    m_next_vertex = nullptr;
    m_pre_point_vertex = nullptr;
    m_next_point_vertex = nullptr;
    m_prim = nullptr;
    m_point = nullptr;
    m_index = INVALID_INDEX;
}

Prim::Prim(PrimType type) : m_vertex(nullptr), m_index(INVALID_INDEX), m_num_vertices(0), m_type(type) {}

void Prim::reset() noexcept
{
    m_vertex = nullptr;
    m_type = PrimType::Polygon;
    m_index = INVALID_INDEX;
    m_num_vertices = 0;
}

void Prim::remove_vertex(Vertex* vertex)
{
    assert(vertex->prim_ptr() == this);
    vertex->m_prim = nullptr;
    --m_num_vertices;
    if(vertex == m_vertex)
    {
        if(m_num_vertices == 0)
        {
            m_vertex = nullptr;
            return;
        }
        m_vertex = m_vertex->next_vertex_ptr();
    }
    if(m_num_vertices == 1)
    {
        m_vertex->m_pre_vertex = nullptr;
        m_vertex->m_next_vertex = nullptr;
    }
    else
    {
        if(vertex->pre_vertex_ptr())
        {
            vertex->pre_vertex_ptr()->m_next_vertex = vertex->next_vertex_ptr();
        }
        if(vertex->next_vertex_ptr())
        {
            vertex->next_vertex_ptr()->m_pre_vertex = vertex->pre_vertex_ptr();
        }
    }
    vertex->m_pre_vertex = nullptr;
    vertex->m_next_vertex = nullptr;
}
    }

}