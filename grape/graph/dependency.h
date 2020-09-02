#ifndef GRAPE_GRAPH_DEPENDENCY_H_
#define GRAPE_GRAPH_DEPENDENCY_H_

#include <memory>
#include <string>
#include <vector>

#include "grape/graph/vertex.h"
#include "grape/graph/edge.h"
#include "grape/graph/adj_list.h"
#include "grape/graph/literal.h"
#include "grape/types.h"


namespace grape {

/**
 * Graph Dependency.
 * This is a class for supporting some graph dependencies,
 * including keys, GFDs, GEDs, GMDs, GMKs,...
 */
template <typename VDATA_T, typename EDATA_T>
class Dependency {

 public:
  Dependency() {}
  ~Dependency() {}

  // vertices.
  inline int GetVerticesNum() const override { return vertices_.size(); }

  inline int GetVertexID(Vertex<int, VDATA_T>& v) { return v.vid(); }

  inline VDATA_T GetVertexData(Vertex<int, VDATA_T>& v) { return v.vdata(); }

  inline int32_t GetVertexLabel(Vertex<int, VDATA_T>& v) { return v.vdata().label_; }

  inline int32_t GetVertexLabel(int vid) { return vertices_[vid].vdata().label_; }

  inline std::vector<std::string> GetVertexAttributes(Vertex<int, VDATA_T>& v) { return v.vdata().attributes_; }

  inline std::vector<Vertex<int, VDATA_T>> GetVertices() { return vertices_; }

  inline void SetVertices(std::vector<Vertex<int, VDATA_T>>& vertices) { vertices_ = vertices; }

  inline std::vector<int> GetAllVerticesID() {
    std::vector<int> vids;
    vids.resize(0);
    for (int i = 0; i < vertices_.size(); i++) {
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
  inline size_t GetEdgeNum() const override { return edges_.size(); }

  inline std::vector<Edge<int, EDATA_T>> GetAllEdges() { return edges_; }

  inline void SetEdges(std::vector<Edge<int, EDATA_T>>& edges) { edges_ = edges; }

  inline std::set<int> GetChildrenID(const int uid) {
    std::set<int> v_children;
    v_children.resize(0);
    for (auto e: edges_) {
      if (e.src() == uid)
        v_children.emplace_back(e.dst());
    }
    return v_children;
  }

  inline std::set<int> GetParentsID(const int uid) {
    std::set<int> v_parents;
    v_parents.resize(0);
    for (auto e: edges_) {
      if (e.dst() == uid)
        v_parents.emplace_back(e.src());
    }
    return v_parents;
  }

  inline EDATA_T GetEdgeLabel(int src, int dst) {
    for (auto e: edges_) {
      if (e.src() == src && e.dst() == dst){
        return e.edata();
      }
    }
  }


 private:
  std::vector<Vertex<int, VDATA_T>> vertices_;
  std::vector<Edge<int, EDATA_T>> edges_;
  std::vector<Literal<EDATA_T>> literals_;
  std::vector<std::vector<int>> important_attrs;

};

}  // namespace grape

#endif  // GRAPE_GRAPH_DEPENDENCY_H_
