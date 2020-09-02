/** Copyright 2020 Alibaba Group Holding Limited.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_H_
#define EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_H_

#include <grape/grape.h>

#include "sssp/sssp_context.h"

namespace grape {

/**
 * @brief SSSP application, determines the length of the shortest paths from a
 * given source vertex to all other vertices in graphs, which can work
 * on both directed and undirected graph.
 *
 * This version of SSSP inherits ParallelAppBase. Messages can be sent in
 * parallel with the evaluation process. This strategy improves the performance
 * by overlapping the communication time and the evaluation time.
 *
 * @tparam FRAG_T
 */
template <typename FRAG_T>
class SSSP : public ParallelAppBase<FRAG_T, SSSPContext<FRAG_T>>,
             public ParallelEngine {
 public:
  // specialize the templated worker.
  INSTALL_PARALLEL_WORKER(SSSP<FRAG_T>, SSSPContext<FRAG_T>, FRAG_T)
  using vertex_t = typename fragment_t::vertex_t;

  /**
   * @brief Partial evaluation for SSSP.
   *
   * @param frag
   * @param ctx
   * @param messages
   */
  void PEval(const fragment_t& frag, context_t& ctx,
             message_manager_t& messages) {
    auto inner_vertices = frag.InnerVertices();
    messages.InitChannels(thread_num());

    vertex_t source;
    bool native_source = frag.GetInnerVertex(ctx.source_id, source);

#ifdef PROFILING
    ctx.exec_time -= GetCurrentTime();
#endif

    /*================================================== for test. ===================================================*/
    if (native_source) {
      LOG(INFO)<<"source lid: " << source.GetValue() << ", source oid: " << frag.GetId(source);
      LOG(INFO)<<"source label: " << frag.GetData(source).label_;
    }
    int32_t vlabel = 1;
    std::vector<std::string> attrs;
    // test whether successfully load label and attrs for vertices, 
    // and test the func 'GetAllVerticesID', 'GetVertexAttributes', 'GetChildrenID' and 'GetParentsID'
    using oid_t = typename fragment_t::oid_t;
    for (auto v: frag.GetAllVerticesID()) {
      std::vector<oid_t> v_children, v_parents;
      if (frag.GetVertexLabel(v, vlabel) && frag.GetVertexAttributes(v, attrs) && frag.GetChildrenID(v, v_children) && frag.GetParentsID(v, v_parents)) {
        std::string tmp_str, tmp_children, tmp_parents;
        for (auto attr: attrs) {
          tmp_str += attr;
          tmp_str += " ";
        }
        for (auto child: v_children) {
          tmp_children += std::to_string(child);
          tmp_children += " ";
        }
        for (auto parent: v_parents) {
          tmp_parents += std::to_string(parent);
          tmp_parents += " ";
        }
        LOG(INFO) << "fid: " << frag.fid() << ", vid: " << v << ", vlabel = " << vlabel
                  << ", attrs: " << tmp_str << ", children oid: " << tmp_children << ", parents oid: " << tmp_parents;
      }
    }
    // test for the func 'GetEdgeLabel'
    using edata_t = typename fragment_t::edata_t;
    edata_t edata = 0;
    bool hasEdge = frag.GetEdgeLabel(11, 16, edata);
    if (hasEdge)
      LOG(INFO) << "fid: " << frag.fid() << ", (11, 16) exist: " << hasEdge << ", elabel: " << edata;
    else
      LOG(INFO) << "fid: " << frag.fid() << " can not obtain edge (11, 16)";
    hasEdge = frag.GetEdgeLabel(11, 13, edata);
    if (hasEdge)
      LOG(INFO) << "fid: " << frag.fid() << ", (11, 13) exist: " << hasEdge << ", elabel: " << edata;
    else
      LOG(INFO) << "fid: " << frag.fid() << " can not obtain edge (11, 13)";
    hasEdge = frag.GetEdgeLabel(16, 12, edata);
    if (hasEdge)
      LOG(INFO) << "fid: " << frag.fid() << ", (16, 12) exist: " << hasEdge << ", elabel: " << edata;
    else
      LOG(INFO) << "fid: " << frag.fid() << " can not obtain edge (16, 12)";
    // test for inner vertices in frag
    for (auto v: frag.GetAllInnerVerticesID()) {
      LOG(INFO) << "fid: " << frag.fid() << ", inner vid: " << v;
    }
    // test for outer vertices in frag
    for (auto v: frag.GetAllOuterVerticesID()) {
      LOG(INFO) << "fid: " << frag.fid() << ", outer vid: " << v;
    }
    // test for whether the frag can get the info of vertex that neither inner nor outer vertex
    if (frag.fid() == 1) {
      if (frag.GetVertexLabel(12, vlabel)) LOG(INFO) << "fid 1, oid 12, label: " << vlabel;
      else LOG(INFO) << "fid 1 can not get the information of oid 12";
      if (frag.GetVertexLabel(16, vlabel)) LOG(INFO) << "fid 1, oid 16, label: " << vlabel;
      else LOG(INFO) << "fid 1 can not get the information of oid 16";
      if (frag.GetVertexLabel(11, vlabel)) LOG(INFO) << "fid 1, oid 11, label: " << vlabel;
      else LOG(INFO) << "fid 1 can not get the information of oid 11";
    }
    // test for edges that hold in the frag
    for (auto v: frag.Vertices()) {
      LOG(INFO) << "fid: " << frag.fid() << ", oid " << frag.GetId(v) << ", isBorderVertex: " << frag.IsBorderVertex(v) 
                << ", IsIncomingBorderVertex: " << frag.IsIncomingBorderVertex(v) << ", IsOutgoingBorderVertex: " << frag.IsOutgoingBorderVertex(v);
      auto src_oid = frag.GetId(v);
      auto es = frag.GetOutgoingAdjList(v);
      for (auto e: es) {
        auto dst_oid = frag.GetId(e.neighbor);
        auto elabel = e.data;
        LOG(INFO) << "fid: " << frag.fid() << ", all outgoing src=" << src_oid << ", dst=" << dst_oid << ", label=" << elabel;
      }
    }
    for (auto v: frag.InnerVertices()) {
      auto src_oid = frag.GetId(v);
      auto es = frag.GetOutgoingAdjList(v);
      for (auto e: es) {
        auto dst_oid = frag.GetId(e.neighbor);
        auto elabel = e.data;
        LOG(INFO) << "fid: " << frag.fid() << ", inner outgoing src=" << src_oid << ", dst=" << dst_oid << ", label=" << elabel;
      }
    }
    for (auto v: frag.OuterVertices()) {
      auto dst_oid = frag.GetId(v);
      auto es = frag.GetIncomingAdjList(v);
      for (auto e: es) {
        auto src_oid = frag.GetId(e.neighbor);
        auto elabel = e.data;
        LOG(INFO) << "fid: " << frag.fid() << ", outer incoming src=" << src_oid << ", dst=" << dst_oid << ", label=" << elabel;
      }
    }
    for (auto v: frag.InnerVertices()) {
      auto dst_oid = frag.GetId(v);
      auto es = frag.GetIncomingAdjList(v);
      for (auto e: es) {
        auto src_oid = frag.GetId(e.neighbor);
        auto elabel = e.data;
        LOG(INFO) << "fid: " << frag.fid() << ", inner incoming src=" << src_oid << ", dst=" << dst_oid << ", label=" << elabel;
      }
    }
    // test 'GetFragId'
    if (frag.fid() == 1) {
      vertex_t v;
      if (frag.GetVertex(12, v))
        LOG(INFO) << "fid 1: oid 12's fid = " << frag.GetFragId(v);
      if (frag.GetVertex(11, v))
        LOG(INFO) << "fid 1: oid 11's fid = " << frag.GetFragId(v);
      else
        LOG(INFO) << "fid 1 can not obtain the info of oid 11";
    }
    /*=================================================== finish test. ===================================================*/

    ctx.next_modified.parallel_clear(thread_num());

    // Get the channel. Messages assigned to this channel will be sent by the
    // message manager in parallel with the evaluation process.
    auto& channel_0 = messages.Channels()[0];

    if (native_source) {
      ctx.partial_result[source] = 0;
      auto es = frag.GetOutgoingAdjList(source);
      for (auto& e : es) {
        vertex_t v = e.neighbor;
        ctx.partial_result[v] = std::min(ctx.partial_result[v], e.data);
        if (frag.IsOuterVertex(v)) {
          // put the message to the channel.
          channel_0.SyncStateOnOuterVertex<fragment_t, double>(
              frag, v, ctx.partial_result[v]);
        } else {
          ctx.next_modified.set_bit(v.GetValue());
        }
      }
    }

#ifdef PROFILING
    ctx.exec_time += GetCurrentTime();
    ctx.postprocess_time -= GetCurrentTime();
#endif

    messages.ForceContinue();

    ctx.next_modified.swap(ctx.curr_modified);
#ifdef PROFILING
    ctx.postprocess_time += GetCurrentTime();
#endif
  }

  /**
   * @brief Incremental evaluation for SSSP.
   *
   * @param frag
   * @param ctx
   * @param messages
   */
  void IncEval(const fragment_t& frag, context_t& ctx,
               message_manager_t& messages) {
    auto inner_vertices = frag.InnerVertices();

    auto& channels = messages.Channels();

#ifdef PROFILING
    ctx.preprocess_time -= GetCurrentTime();
#endif

    ctx.next_modified.parallel_clear(thread_num());

    // parallel process and reduce the received messages
    messages.ParallelProcess<fragment_t, double>(
        thread_num(), frag, [&ctx](int tid, vertex_t u, double msg) {
          if (ctx.partial_result[u] > msg) {
            atomic_min(ctx.partial_result[u], msg);
            ctx.curr_modified.set_bit(u.GetValue());
          }
        });

#ifdef PROFILING
    ctx.preprocess_time += GetCurrentTime();
    ctx.exec_time -= GetCurrentTime();
#endif

    // incremental evaluation.
    ForEach(ctx.curr_modified, inner_vertices,
            [&frag, &ctx](int tid, vertex_t v) {
              double distv = ctx.partial_result[v];
              auto es = frag.GetOutgoingAdjList(v);
              for (auto& e : es) {
                vertex_t u = e.neighbor;
                double ndistu = distv + e.data;
                if (ndistu < ctx.partial_result[u]) {
                  atomic_min(ctx.partial_result[u], ndistu);
                  ctx.next_modified.set_bit(u.GetValue());
                }
              }
            });

    // put messages into channels corresponding to the destination fragments.

#ifdef PROFILING
    ctx.exec_time += GetCurrentTime();
    ctx.postprocess_time -= GetCurrentTime();
#endif
    auto outer_vertices = frag.OuterVertices();
    ForEach(ctx.next_modified, outer_vertices,
            [&channels, &frag, &ctx](int tid, vertex_t v) {
              channels[tid].SyncStateOnOuterVertex<fragment_t, double>(
                  frag, v, ctx.partial_result[v]);
            });

    if (!ctx.next_modified.partial_empty(0, frag.GetInnerVerticesNum())) {
      messages.ForceContinue();
    }

    ctx.next_modified.swap(ctx.curr_modified);
#ifdef PROFILING
    ctx.postprocess_time += GetCurrentTime();
#endif
  }
};

}  // namespace grape

#endif  // EXAMPLES_ANALYTICAL_APPS_SSSP_SSSP_H_
