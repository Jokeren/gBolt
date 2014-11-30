#ifndef DATABASE_H
#define DATABASE_H
#include "graph.h"

namespace gspan {
	class DataBase {
		public:
			static DataBase *get_instance() {
				return _m_instance;
			}

			void push_graph(const Graph& graph) {
				_m_graph.push_back(graph);
			}

			std::vector<Graph> get_graph() {
				return _m_graph;
			}

			const char* c_str();

		private:
			std::vector<Graph> _m_graph;
			static DataBase *_m_instance;
			DataBase(){};
	};
}//namespace gspan

#endif
