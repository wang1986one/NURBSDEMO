#pragma once
#include "mesh_element.h"
#include"range.h"
#include<unordered_set>

/*
namespace Geomerty
{
   

struct MeshIndexRange : public ranges::RangeBase<MeshIndexRange>
{
    virtual ~MeshIndexRange() = default;

    size_t value() const noexcept { return m_idx; }
    bool at_end() const noexcept { return m_at_end; }
    void advance() noexcept { advance_impl(); }
    size_t idx() const noexcept { return m_idx; }

    virtual std::shared_ptr<MeshIndexRange> clone() const noexcept = 0;

protected:
    virtual void advance_impl() = 0;
    explicit MeshIndexRange(size_t idx = INVALID_INDEX);

    size_t m_idx = INVALID_INDEX;
    bool m_at_end = false;
};


struct EdgeRange : public ranges::RangeBase<EdgeRange>
{
    explicit EdgeRange(const NMesh* mesh);
    virtual ~EdgeRange() = default;

    Edge value() const noexcept { return *m_current_edge; }
     bool at_end() const noexcept { return m_current_edge == m_edge_set.end(); }
     void advance() noexcept { ++m_current_edge; }

protected:
    std::unordered_set<Edge> m_edge_set;
    std::unordered_set<Edge>::iterator m_current_edge;
};


struct  VertexLoopRange : public MeshIndexRange
{
    VertexLoopRange(const NMesh* mesh, size_t vertex_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<VertexLoopRange>(*this); }

private:
    void advance_impl() override;

    const Vertex* m_start_vtx;
    const Vertex* m_vtx;
};


struct  PrimVerticesRange : public VertexLoopRange
{
    PrimVerticesRange(const NMesh* mesh, size_t prim_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<PrimVerticesRange>(*this); }
};


struct  PrimPointsRange : public MeshIndexRange
{
    PrimPointsRange(const NMesh* mesh, size_t prim_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<PrimPointsRange>(*this); }

private:
    void advance_impl() override;

    const Vertex* m_start_vertex;
    const Vertex* m_vertex;
};


struct PointVerticesRange : public MeshIndexRange
{
    PointVerticesRange(const NMesh* mesh, size_t point_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<PointVerticesRange>(*this); }

private:
    void advance_impl() override;

    const Vertex* m_vertex;
};


struct  PointPrimsRange : public MeshIndexRange
{
    PointPrimsRange(const NMesh* mesh, size_t point_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<PointPrimsRange>(*this); }

private:
    void advance_impl() override;

    const Vertex* m_vertex;
};


struct  PointNeighboursRange : public MeshIndexRange
{
    PointNeighboursRange(const NMesh* mesh, size_t point_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<PointNeighboursRange>(*this); }

private:
    void advance_impl() override;

    const Point* m_point;
    const Vertex* m_vertex;
    std::vector<size_t> m_used_indices;
    bool m_flag;
};


struct  PrimNeighboursRange : public MeshIndexRange
{
    PrimNeighboursRange(const NMesh* mesh, size_t prim_idx);

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<PrimNeighboursRange>(*this); }

private:
    void advance_impl() override;

    const Vertex* m_start_vertex;
    const Vertex* m_vertex;
    const NMesh* m_mesh;
    Array<index_t> m_used_indices;
    PointPrimsRange m_current_point_prims_r;
};

struct IndexSequenceRange : public MeshIndexRange
{
    explicit IndexSequenceRange(const size_t start, const size_t end) : m_end(end) { m_idx = start; }

    explicit IndexSequenceRange(const size_t end) : m_end(end) { m_idx = 0; }

    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<IndexSequenceRange>(*this); }

private:
    void advance_impl() override
    {
        if(++m_idx >= m_end)
        {
            m_at_end = true;
        }
    }

    const size_t m_end;
};


template<typename Iter> struct MeshIndexRangeAdapter : public MeshIndexRange
{
    MeshIndexRangeAdapter(Iter begin, const Iter& end) : MeshIndexRange(), current_it(begin), end_it(end)
    {
        if(current_it != end_it)
        {
            m_idx = *current_it;
        }
        else
        {
            m_at_end = true;
        }
    }
    std::shared_ptr<MeshIndexRange> clone() const noexcept override { return std::make_shared<MeshIndexRangeAdapter>(*this); }

private:
    void advance_impl() override
    {
        if(++current_it != end_it)
        {
            m_idx = *current_it;
        }
        else
        {
            m_at_end = true;
        }
    };

    Iter current_it;
    const Iter end_it;
};

}// namespace nous

*/
