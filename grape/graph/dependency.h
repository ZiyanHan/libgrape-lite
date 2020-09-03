#ifndef GRAPE_GRAPH_DEPENDENCY_H_
#define GRAPE_GRAPH_DEPENDENCY_H_

#include <memory>
#include <string>
#include <vector>

#include "grape/types.h"
#include "grape/graph/vertex.h"
#include "grape/graph/edge.h"
#include "grape/graph/literal.h"

#include <grape/grape.h>

namespace grape {

/**
 * Graph Dependency.
 * This is a class for supporting all graph dependencies,
 * including keys, GFDs, GEDs, GMDs, GMKs,...
 */
template <typename VDATA_T, typename EDATA_T>
class Dependency {

 public:
  using internal_vertex_t = internal::Vertex<int, VDATA_T>;
  Dependency() {}
  ~Dependency() {}

  // vertices.
  inline size_t GetVerticesNum() const { return vertices_.size(); }

  inline int GetVertexID(internal_vertex_t& v) { return v.vid(); }

  inline VDATA_T GetVertexData(internal_vertex_t& v) { return v.vdata(); }

  inline int32_t GetVertexLabel(internal_vertex_t& v) { return v.vdata().label_; }

  inline int32_t GetVertexLabel(int vid) { return vertices_[vid].vdata().label_; }

  inline std::vector<std::string> GetVertexAttributes(internal_vertex_t& v) { return v.vdata().attributes_; }

  inline std::vector<internal_vertex_t> GetVertices() { return vertices_; }

  inline void SetVertices(std::vector<internal_vertex_t>& vertices) { vertices_ = vertices; }

  inline std::vector<int> GetAllVerticesID() {
    std::vector<int> vids;
    vids.resize(0);
    for (size_t i = 0; i < vertices_.size(); i++) {
      vids.emplace_back(i);
    }
    return vids;
  }

  // literals.
  inline void SetImportantAttrs(std::vector<std::vector<int>> attrs) { important_attrs = attrs; }

  inline std::vector<std::vector<int>> GetImportantAttrs() { return important_attrs; }

  inline size_t GetLiteralsNum() { return literals_.size(); }

  inline void SetLiterals(std::vector<Literal<EDATA_T>>& literal) { literals_ = literal; }

  inline std::vector<Literal<EDATA_T>>& GetLiterals() { return literals_; }

  inline Literal<EDATA_T>& GetLiteral(int lid) { assert(lid < literals_.size());  return literals_[lid]; }

  // edges.
  inline size_t GetEdgeNum() const { return edges_.size(); }

  inline std::vector<Edge<int, EDATA_T>> GetAllEdges() { return edges_; }

  inline void SetEdges(std::vector<Edge<int, EDATA_T>>& edges) { edges_ = edges; }

  inline std::set<int> GetChildrenID(const int uid) {
    std::set<int> v_children;
    v_children.clear();
    for (auto e: edges_) {
      if (e.src() == uid)
        v_children.insert(e.dst());
    }
    return v_children;
  }

  inline std::set<int> GetParentsID(const int uid) {
    std::set<int> v_parents;
    v_parents.clear();
    for (auto e: edges_) {
      if (e.dst() == uid)
        v_parents.insert(e.src());
    }
    return v_parents;
  }

  inline bool GetEdgeLabel(int src, int dst, EDATA_T &elabel) {
    for (auto e: edges_) {
      if (e.src() == src && e.dst() == dst){
        elabel = e.edata();
        return true;
      }
    }
    return false;
  }


 private:
  std::vector<internal_vertex_t> vertices_;
  std::vector<Edge<int, EDATA_T>> edges_;
  std::vector<Literal<EDATA_T>> literals_;
  std::vector<std::vector<int>> important_attrs;

};

}  // namespace grape

#endif  // GRAPE_GRAPH_DEPENDENCY_H_
