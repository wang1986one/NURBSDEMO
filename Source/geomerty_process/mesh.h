#pragma once
#include"mat_vec.h"
#include "mesh_element.h"
#include "mesh_index_range.h"
/*
namespace Geomerty
{
class  NMesh
{
	//N_NON_COPYABLE(NMesh)
public:
	static std::shared_ptr<NMesh> create();

	explicit NMesh();
	~NMesh() = default;

	// add point
	size_t add_point(float pos_x, float pos_y, float pos_z);
	size_t add_point(const vec3& pos);
	size_t add_points(size_t size);
	size_t add_points(const std::vector<vec3>& pos_arr);

	// add vertex
	size_t add_vertex(size_t pre_vertex_idx);
	size_t add_vertex(size_t prim_idx, uint_t idx_in_prim);
	size_t add_vertex(size_t prim_idx, size_t point_idx);
	size_t add_vertices(size_t size);

	// add prim
	size_t add_prim(const std::vector<Point*>& points, PrimType prim_type = PrimType::Polygon);
	size_t add_prim(const std::vector<vec3>& point_pos, PrimType prim_type = PrimType::Polygon);
	size_t add_prims(size_t size, uint_t vertex_count_per_prim = 0, PrimType prim_type = PrimType::Polygon);

	// delete element
	void delete_point(size_t idx);
	void delete_vertex(size_t idx, bool with_point = true);
	void delete_prim(size_t idx, bool with_points = true);
	void delete_unused_points();

	// get element
	  Point* point(size_t idx) noexcept { return m_points[idx]; }
	  Vertex* vertex(size_t idx) noexcept { return m_vertices[idx]; }
	  Prim* prim(size_t idx) noexcept { return m_prims[idx]; }
	  const Point* point(size_t idx) const noexcept { return m_points[idx]; }
	  const Vertex* vertex(size_t idx) const noexcept { return m_vertices[idx]; }
	  const Prim* prim(size_t idx) const noexcept { return m_prims[idx]; }

	// topology
	uint_t prim_num_vertices(size_t prim_idx) const;
	void wire_vertex_prim(size_t vertex_idx, size_t pre_vertex_idx);
	void wire_vertex_prim(size_t vertex_idx, size_t prim_idx, uint_t idx_in_prim);
	void wire_vertex_point(size_t vertex_idx, size_t point_idx);
	void link_vertex(size_t from_vertex_idx, size_t to_vertex_idx);
	void split_vertex(size_t vertex_idx);
	void split_all_vertices();
	// If deleted_points is not nullptr, it will be filled with the points needs to delete.
	void merge_point(const size_t from_point_idx, const size_t to_point_idx, SortedSet<size_t>* need_delete_points = nullptr);

	// ranges
	  VertexLoopRange vertex_loop_range(size_t vertex_idx) const { return { this, vertex_idx }; }
	  PrimVerticesRange prim_vertices_range(size_t prim_idx) const { return { this, prim_idx }; }
	  PrimPointsRange prim_points_range(size_t prim_idx) const { return { this, prim_idx }; }
	  PrimNeighboursRange prim_neighbours_range(size_t prim_idx) const { return { this, prim_idx }; }
	  PointVerticesRange point_vertices_range(size_t point_idx) const { return { this, point_idx }; }
	  PointPrimsRange point_prims_range(size_t point_idx) const { return { this, point_idx }; }
	  PointNeighboursRange point_neighbours_range(size_t point_idx) const { return { this, point_idx }; }
	  EdgeRange edges_range() const { return EdgeRange{ this }; }

	  auto point_index_range() const noexcept { return m_points.index_range(); }
	  auto vertex_index_range() const noexcept { return m_vertices.index_range(); }
	  auto prim_index_range() const noexcept { return m_prims.index_range(); }
	  auto index_range(AttributeClass attrib_cls) const noexcept
	{
		switch(attrib_cls)
		{
		case AttributeClass::Point: return m_points.index_range();
		case AttributeClass::Vertex: return m_vertices.index_range();
		case AttributeClass::Prim: return m_prims.index_range();
		case AttributeClass::Global: return IndexRange<size_t>(1);
		default: return IndexRange<size_t>(0);
		}
	}

	  auto point_blocked_range() const noexcept { return m_points.blocked_range(); }
	  auto vertex_blocked_range() const noexcept { return m_vertices.blocked_range(); }
	  auto prim_blocked_range() const noexcept { return m_points.blocked_range(); }
	  auto blocked_range(AttributeClass attrib_cls) const noexcept
	{
		switch(attrib_cls)
		{
		case AttributeClass::Point: return m_points.blocked_range();
		case AttributeClass::Vertex: return m_vertices.blocked_range();
		case AttributeClass::Prim: return m_prims.blocked_range();
		case AttributeClass::Global: return internal::PageBlockedRange(0, 1);
		default: return internal::PageBlockedRange(0, 0);
		}
	}

	// mesh element info
	  size_t num_points() const noexcept { return m_points.size(); }
	  size_t num_vertices() const noexcept { return m_vertices.size(); }
	  size_t num_prims() const noexcept { return m_prims.size(); }
	  size_t num_elements(AttributeClass attrib_cls) const noexcept
	{
		switch(attrib_cls)
		{
		case AttributeClass::Point: return m_points.size();
		case AttributeClass::Vertex: return m_vertices.size();
		case AttributeClass::Prim: return m_prims.size();
		case AttributeClass::Global: return 1;
		default: return 0;
		}
	}

	 uint64 memory_usage() const noexcept;
	  uint64 points_memory_usage() const noexcept { return m_points.memory_usage(); }
	  uint64 vertices_memory_usage() const noexcept { return m_vertices.memory_usage(); }
	  uint64 prims_memory_usage() const noexcept { return m_prims.memory_usage(); }

	// copy & merge
	void copy(const NMesh* src_mesh, bool only_copy_points = false, bool copy_attribs = true);
	void shallow_copy(const NMesh* src_mesh, bool only_copy_points = false, bool copy_attribs = true);
	void merge(const NMesh* src_mesh, bool only_merge_points = false, bool merge_attribs = true);

	// shared
	void ensure_topology_unique();
	void ensure_all_attribs_unique();
	 void ensure_point_attribs_unique() { ensure_attribs_unique(m_point_attribs); }
	 void ensure_vertex_attribs_unique() { ensure_attribs_unique(m_vertex_attribs); }
	 void ensure_prim_attribs_unique() { ensure_attribs_unique(m_prim_attribs); }
	 void ensure_global_attribs_unique() { ensure_attribs_unique(m_global_attribs); }
	  bool is_topology_shared() const noexcept { return m_points.is_shared(); }

	// mesh utils
	 void clear(bool preserve_points = false) { clear(preserve_points, true); }
	  bool empty() const noexcept
	{
		return m_points.empty() && m_vertices.empty() && m_prims.empty() && m_global_attribs.empty();
	}
	 DataVersion topology_version() const noexcept;
	void bump_topology_version() noexcept { ++m_topology_version; };
	 AABB3 bbox() const;
	 vec3 pivot() const;
	 AABB3 prim_bbox(size_t prim_idx) const;
	void reorder_elements(const std::vector<size_t>& indices, AttributeClass cls, bool with_attribs = true);

	// boundary
	 bool is_edge_on_boundary(Edge edge) const;
	 bool is_point_on_boundary(size_t point_idx) const;
	 bool is_prim_on_boundary(size_t prim_idx) const;

	// poly info
	 float prim_area(size_t prim_idx) const;
	 vec3 prim_barycenter(size_t prim_idx) const;
	 vec3 prim_normal(size_t prim_idx) const;
	 vec3 prim_normal_unnormalized(size_t prim_idx) const;
	 static vec3 prim_normal_unnormalized(const Prim* prim, const Attribute<vec3>* pos_attrib);

	// create attribute
	template<typename T> Attribute<T>* create_attrib(AttributeClass attrib_cls, StringView attrib_name, const T& default_value = T());
	template<typename T> Attribute<T>* create_point_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T> Attribute<T>* create_vertex_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T> Attribute<T>* create_prim_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T> Attribute<T>* create_global_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T> unique_ptr<Attribute<T>> create_scoped_attrib(AttributeClass attrib_cls, const T& default_value = T());

	// get attribute
	 AttributeBase* attrib(AttributeClass attrib_cls, StringView attrib_name);
	 AttributeBase* point_attrib(StringView attrib_name);
	 AttributeBase* vertex_attrib(StringView attrib_name);
	 AttributeBase* prim_attrib(StringView attrib_name);
	 AttributeBase* global_attrib(StringView attrib_name);

	 const AttributeBase* attrib(AttributeClass attrib_cls, StringView attrib_name) const;
	 const AttributeBase* point_attrib(StringView attrib_name) const;
	 const AttributeBase* vertex_attrib(StringView attrib_name) const;
	 const AttributeBase* prim_attrib(StringView attrib_name) const;
	 const AttributeBase* global_attrib(StringView attrib_name) const;

	template<typename T>  Attribute<T>* attrib(AttributeClass attrib_cls, StringView attrib_name);
	template<typename T>  Attribute<T>* point_attrib(StringView attrib_name);
	template<typename T>  Attribute<T>* vertex_attrib(StringView attrib_name);
	template<typename T>  Attribute<T>* prim_attrib(StringView attrib_name);
	template<typename T>  Attribute<T>* global_attrib(StringView attrib_name);

	template<typename T>  const Attribute<T>* attrib(AttributeClass attrib_cls, StringView attrib_name) const;
	template<typename T>  const Attribute<T>* point_attrib(StringView attrib_name) const;
	template<typename T>  const Attribute<T>* vertex_attrib(StringView attrib_name) const;
	template<typename T>  const Attribute<T>* prim_attrib(StringView attrib_name) const;
	template<typename T>  const Attribute<T>* global_attrib(StringView attrib_name) const;

	// get or create attribute
	template<typename T>
	 Attribute<T>* get_or_create_attrib(AttributeClass attrib_cls, StringView attrib_name, const T& default_value = T());
	template<typename T>  Attribute<T>* get_or_create_point_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T>  Attribute<T>* get_or_create_vertex_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T>  Attribute<T>* get_or_create_prim_attrib(StringView attrib_name, const T& default_value = T());
	template<typename T>  Attribute<T>* get_or_create_global_attrib(StringView attrib_name, const T& default_value = T());

	// get attribute map
	 AttributeMap& attribs(AttributeClass attrib_cls) noexcept;
	  AttributeMap& point_attribs() noexcept { return m_point_attribs; }
	  AttributeMap& vertex_attribs() noexcept { return m_vertex_attribs; }
	  AttributeMap& prim_attribs() noexcept { return m_prim_attribs; }
	  AttributeMap& global_attribs() noexcept { return m_global_attribs; }

	 const AttributeMap& attribs(AttributeClass attrib_cls) const noexcept;
	  const AttributeMap& point_attribs() const noexcept { return m_point_attribs; }
	  const AttributeMap& vertex_attribs() const noexcept { return m_vertex_attribs; }
	  const AttributeMap& prim_attribs() const noexcept { return m_prim_attribs; }
	  const AttributeMap& global_attribs() const noexcept { return m_global_attribs; }

	// global attribute
	void copy_global_attribs(const NMesh* src_mesh);
	void merge_global_attribs(const NMesh* src_mesh);

	// attribute utils
	void add_attrib(AttributeClass attrib_cls, StringView attrib_name, AttributeBasePtr attrib_ptr);
	void remove_attrib(AttributeClass attrib_cls, StringView attrib_name);
	void remove_attrib(AttributeClass attrib_cls, AttributeBase* attrib);
	void rename_attrib(AttributeClass attrib_cls, StringView from, StringView to);
	void swap_attrib(AttributeClass attrib_cls, StringView attrib_name_a, StringView attrib_name_b, bool copy_if_not_exist = true);
	void copy_attrib(AttributeClass attrib_cls, StringView src_attrib_name, StringView dst_attrib_name);

	// attribute info
	 std::vector<String> attrib_names(AttributeClass attrib_cls) const;
	 std::vector<StringView> attrib_names_ref(AttributeClass attrib_cls) const;
	  size_t num_attribs(AttributeClass attrib_cls) const { return attribs(attrib_cls).size(); }
	  bool has_attrib(AttributeClass attrib_cls, StringView attrib_name) const
	{
		return attribs(attrib_cls).contains(attrib_name);
	}



private:
	template<typename T> Attribute<T>* create_attrib(AttributeMap& attrib_map, StringView attrib_name, const T& default_value, const size_t* size);
	template<typename T>
	Attribute<T>* get_or_create_attrib(AttributeMap& attrib_map, StringView attrib_name, const T& default_value, const size_t* size);
	template<typename T> Attribute<T>* get_attrib(const AttributeMap& attrib_map, StringView attrib_name) const;
	 static AttributeBase* get_attrib(const AttributeMap& attrib_map, StringView attrib_name);
	const size_t* get_attrib_size(AttributeClass attrib_cls);
	void clear(bool preserve_points, bool preserve_pos_attrib);
	static void ensure_attribs_unique(AttributeMap& attrib_map);
	void merge_attribs(AttributeClass cls, const AttributeMap& src_attribs, size_t start_idx, size_t size);
	void merge_topology(const NMesh* src_mesh, size_t point_idx_offset, size_t vertex_idx_offset, size_t prim_idx_offset);
	void build_topology(const PointContainer::array_type& point_container, const VertexContainer::array_type& vertex_container,
						const PrimContainer::array_type& prim_container, size_t point_idx_offset, size_t vertex_idx_offset,
						size_t prim_idx_offset);
	void delete_point(Point* point);
	void delete_vertex(Vertex* vertex);
	void delete_prim(Prim* prim);

	// serialize
	void save_attribs(Serializer& serializer, AttributeClass cls) const;
	void load_attribs(Serializer& serializer, AttributeClass cls);

private:
	std::vector<Point> m_points;
	std::vector<Vertex> m_vertices;
	std::vector<Prim> m_prims;

	AttributeMap m_point_attribs;
	AttributeMap m_vertex_attribs;
	AttributeMap m_prim_attribs;
	AttributeMap m_global_attribs;

	PosAttribute* m_pos_attrib;
	uint64 m_topology_version;
};
}// namespace nous

namespace nous
{
template<typename T>
inline Attribute<T>* NMesh::create_attrib(AttributeMap& attrib_map, StringView attrib_name, const T& default_value, const size_t* size)
{
	if(attrib_name.empty())
	{
		return nullptr;
	}
	if(&attrib_map == &m_point_attribs && m_pos_attrib && attrib_name == ATTRIB_NAME_POS)
	{
		if constexpr(stl::is_same_v<T, vec3>)
		{
			return m_pos_attrib;
		}
		else
		{
			return nullptr;
		}
	}
	auto attrib = Attribute<T>::create(size, default_value);
	attrib_map.insert_or_assign(attrib_name, to_shared_ptr<AttributeBase>(attrib));
	return attrib;
}

inline const size_t* NMesh::get_attrib_size(AttributeClass attrib_cls)
{
	const size_t* size;
	switch(attrib_cls)
	{
	case AttributeClass::Point: size = m_points.size_ref(); break;
	case AttributeClass::Vertex: size = m_vertices.size_ref(); break;
	case AttributeClass::Prim: size = m_prims.size_ref(); break;
	case AttributeClass::Global: size = &global_attrib_size; break;
	}
	return size;
}

template<typename T> inline Attribute<T>* NMesh::create_attrib(AttributeClass attrib_cls, StringView attrib_name, const T& default_value)
{
	return create_attrib<T>(attribs(attrib_cls), attrib_name, default_value, get_attrib_size(attrib_cls));
}

template<typename T> inline Attribute<T>* NMesh::create_point_attrib(StringView attrib_name, const T& default_value)
{
	return create_attrib<T>(m_point_attribs, attrib_name, default_value, m_points.size_ref());
}

template<typename T> inline Attribute<T>* NMesh::create_vertex_attrib(StringView attrib_name, const T& default_value)
{
	return create_attrib<T>(m_vertex_attribs, attrib_name, default_value, m_vertices.size_ref());
}

template<typename T> inline Attribute<T>* NMesh::create_prim_attrib(StringView attrib_name, const T& default_value)
{
	return create_attrib<T>(m_prim_attribs, attrib_name, default_value, m_prims.size_ref());
}

template<typename T> inline Attribute<T>* NMesh::create_global_attrib(StringView attrib_name, const T& default_value)
{
	return create_attrib<T>(m_global_attribs, attrib_name, default_value, &global_attrib_size);
}

template<typename T> inline unique_ptr<Attribute<T>> NMesh::create_scoped_attrib(AttributeClass attrib_cls, const T& default_value)
{
	return to_unique_ptr<Attribute<T>>(Attribute<T>::create(get_attrib_size(attrib_cls), default_value));
}

template<typename T> inline Attribute<T>* NMesh::get_attrib(const AttributeMap& attrib_map, StringView attrib_name) const
{
	if(auto attrib = get_attrib(attrib_map, attrib_name))
	{
		if(attrib->is_type<T>())
		{
			return attrib->cast<T>();
		}
	}
	return nullptr;
}

template<typename T> inline Attribute<T>* NMesh::attrib(AttributeClass attrib_cls, StringView attrib_name)
{
	return get_attrib<T>(attribs(attrib_cls), attrib_name);
}

template<typename T> inline Attribute<T>* NMesh::point_attrib(StringView attrib_name) { return get_attrib<T>(m_point_attribs, attrib_name); }

template<typename T> inline Attribute<T>* NMesh::vertex_attrib(StringView attrib_name) { return get_attrib<T>(m_vertex_attribs, attrib_name); }

template<typename T> inline Attribute<T>* NMesh::prim_attrib(StringView attrib_name) { return get_attrib<T>(m_prim_attribs, attrib_name); }

template<typename T> inline Attribute<T>* NMesh::global_attrib(StringView attrib_name) { return get_attrib<T>(m_global_attribs, attrib_name); }

template<typename T> inline const Attribute<T>* NMesh::attrib(AttributeClass attrib_cls, StringView attrib_name) const
{
	return get_attrib<T>(attribs(attrib_cls), attrib_name);
}

template<typename T> inline const Attribute<T>* NMesh::point_attrib(StringView attrib_name) const
{
	return get_attrib<T>(m_point_attribs, attrib_name);
}

template<typename T> inline const Attribute<T>* NMesh::vertex_attrib(StringView attrib_name) const
{
	return get_attrib<T>(m_vertex_attribs, attrib_name);
}

template<typename T> inline const Attribute<T>* NMesh::prim_attrib(StringView attrib_name) const
{
	return get_attrib<T>(m_prim_attribs, attrib_name);
}

template<typename T> inline const Attribute<T>* NMesh::global_attrib(StringView attrib_name) const
{
	return get_attrib<T>(m_global_attribs, attrib_name);
}

template<typename T>
inline Attribute<T>* NMesh::get_or_create_attrib(AttributeMap& attrib_map, StringView attrib_name, const T& default_value, const size_t* size)
{
	if(auto attrib = get_attrib<T>(attrib_map, attrib_name))
	{
		return attrib;
	}
	return create_attrib<T>(attrib_map, attrib_name, default_value, size);
}

template<typename T> inline Attribute<T>* NMesh::get_or_create_attrib(AttributeClass attrib_cls, StringView attrib_name, const T& default_value)
{
	return get_or_create_attrib<T>(attribs(attrib_cls), attrib_name, default_value, get_attrib_size(attrib_cls));
}

template<typename T> inline Attribute<T>* NMesh::get_or_create_point_attrib(StringView attrib_name, const T& default_value)
{
	return get_or_create_attrib<T>(m_point_attribs, attrib_name, default_value, m_points.size_ref());
}

template<typename T> inline Attribute<T>* NMesh::get_or_create_vertex_attrib(StringView attrib_name, const T& default_value)
{
	return get_or_create_attrib<T>(m_vertex_attribs, attrib_name, default_value, m_vertices.size_ref());
}

template<typename T> inline Attribute<T>* NMesh::get_or_create_prim_attrib(StringView attrib_name, const T& default_value)
{
	return get_or_create_attrib<T>(m_prim_attribs, attrib_name, default_value, m_prims.size_ref());
}

template<typename T> inline Attribute<T>* NMesh::get_or_create_global_attrib(StringView attrib_name, const T& default_value)
{
	return get_or_create_attrib<T>(m_global_attribs, attrib_name, default_value, &global_attrib_size);
}

namespace attribute_create::detail
{
template<typename T> AttributeBase* attribute_creator(NMesh* mesh, AttributeClass attrib_cls, StringView attrib_name)
{
	return mesh->create_attrib<T>(attrib_cls, attrib_name);
}
}// namespace attribute_create::detail
}// namespace nous
*/

