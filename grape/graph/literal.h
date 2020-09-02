#ifndef GRAPE_GRAPH_LITERAL_H_
#define GRAPE_GRAPH_LITERAL_H_

#include <memory>
#include <string>
#include <vector>
#include "grape/util.h"


namespace grape {

/**
 * Literals of LPR dependencies.
 */
template <typename EDATA_T>
class Literal {
 public:
  Literal() {}

  // x.A = y.B
  Literal(int vertex_x, int vertex_y, int attribute_id_x, int attribute_id_y) {
    vid_x_ = vertex_x;
    vid_y_ = vertex_y;
    aid_x_ = attribute_id_x;
    aid_y_ = attribute_id_y;
    constant_ = "";
    edge_label_ = -1;
    literal_type_ = 0;
  }

  // x.A = c
  Literal(int vertex_x, int attribute_id_x, std::string constant){
    vid_x_ = vertex_x;
    vid_y_ = -1;
    aid_x_ = attribute_id_x;
    aid_y_ = -1;
    constant_ = constant;
    edge_label_ = -1;
    literal_type_ = 1;
  }

  // x.A
  Literal(int vertex_x, int attribute_id_x){
    vid_x_ = vertex_x;
    vid_y_ = -1;
    aid_x_ = attribute_id_x;
    aid_y_ = -1;
    constant_ = "";
    edge_label_ = -1;
    literal_type_ = 2;
  }

  // l(x,y)
  // here the parameter 'link_tag' is useless, just for the convenience of distinguishing from M(x,y,l)
  Literal(int vertex_x, int vertex_y, EDATA_T edgelabel, int link_tag){
    vid_x_ = vertex_x;
    vid_y_ = vertex_y;
    aid_x_ = -1;
    aid_y_ = -1;
    constant_ = "";
    edge_label_ = edgelabel;
    literal_type_ = 3;
  }

  // M(x,y,l)
  Literal(int vertex_x, int vertex_y, EDATA_T edgelabel){
    vid_x_ = vertex_x;
    vid_y_ = vertex_y;
    aid_x_ = -1;
    aid_y_ = -1;
    constant_ = "";
    edge_label_ = edgelabel;
    literal_type_ = 4;
  }

  /**
   * Create a literal from one line of string
   * @param  line  one string with all information of this literal,
   * which excludes the letter "l" entered at the beginning in the dependency file.
   */
  explicit Literal(const std::string line) {
    std::vector<std::string> items = split(line, "\t");
    if (items[0] == "0") {                                               // x.A = y.B
      vid_x_ = std::stoll(items[1]);
      vid_y_ = std::stoll(items[2]);
      aid_x_ = std::stoi(items[3]);
      aid_y_ = std::stoi(items[4]);
      literal_type_ = 0;
      constant_ = "";
      edge_label_ = -1;
    } else if (items[0] == "1") {                                        // x.A = c
      vid_x_ = std::stoll(items[1]);
      aid_x_ = std::stoi(items[2]);
      constant_ = items[3];
      literal_type_ = 1;
      vid_y_ = -1;
      aid_y_ = -1;
      edge_label_ = -1;
    } else if (items[0] == "2") {                                        // x.A
      vid_x_ = std::stoll(items[1]);
      aid_x_ = std::stoi(items[2]);
      literal_type_ = 2;
      vid_y_ = -1;
      aid_y_ = -1;
      constant_ = "";
      edge_label_ = -1;
    } else if (items[0] == "3") {                                       // l(x,y)
      vid_x_ = std::stoll(items[1]);
      vid_y_ = std::stoll(items[2]);
      edge_label_ = std::stoi(items[3]);
      literal_type_ = 3;
      aid_x_ = -1;
      aid_y_ = -1;
      constant_ = "";
    } else if (items[0] == "4") {                                       // M(x,y,l)
      vid_x_ = std::stoll(items[1]);
      vid_y_ = std::stoll(items[2]);
      edge_label_ = std::stoi(items[3]);
      literal_type_ = 4;
      aid_x_ = -1;
      aid_y_ = -1;
      constant_ = "";
    } else {
      LOG(INFO) << "The literal input here is invalid.\n"
                << "Skip this literal.";
    }
  }

  /**
   * Get vertex X id.
   */
  inline int getVertexX() { return vid_x_; }

  /**
   * Get vertex Y id.
   */
  inline int getVertexY() { return vid_y_; }

  /**
   * Get the attribute id of the vertex x.
   */
  inline int getAttributeIdX() { return aid_x_; }

  /**
   * Get the attribute id of the vertex y.
   */
  inline int getAttributeIdY() { return aid_y_; }

  /**
   * Get the constant value for the literal in the form of "x.A = c".
  */
  inline std::string getConstant() { return constant_; }

  /**
   * Get the edge label in the form of "l(x,y)" or "ML(x,y,l)".
  */
  inline EDATA_T getEdgeLabel() { return edge_label_; }

  /**
   * Get the type of the literal.
   * 0 : x.A = y.B
   * 1 : x.A = c
   * 2 : x.A
   * 3 : l(x,y)
   * 4 : M(x,y,l)
  */
  inline int getLiteralType() { return literal_type_; }

  bool operator==(const Literal& literal) const {
    return (this->vid_x_ == literal.vid_x_) && (this->vid_y_ == literal.vid_y_) && (this->aid_x_ == literal.aid_x_) && 
           (this->aid_y_ == literal.aid_y_) && (this->constant_ == literal.constant_) && 
           (this->edge_label_ == literal.edge_label_) && (this->literal_type_ == literal.literal_type_);
  }

 private:
  int vid_x_;
  int vid_y_;
  int aid_x_;
  int aid_y_;                   // x.A=y.B
  std::string constant_;        // x.A=c
  EDATA_T edge_label_;          // l(x,y) or ML(x,y,l)
  int literal_type_;            // five types of literals in LPR
};

}  // namespace grape

#endif  // GRAPE_GRAPH_LITERAL_H_
