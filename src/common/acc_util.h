
std::string convertSym(const nnvm::Symbol& sym) {
  nnvm::Symbol ret = sym.Copy();
  nnvm::Graph g;
  g.outputs = ret.outputs;

  std::string json(nnvm::pass::SaveJSON(g));
  return json;
}
