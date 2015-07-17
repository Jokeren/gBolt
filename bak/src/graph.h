#ifndef GRAPH_H
#define GRAPH_H
#include "common.h"

namespace gspan {
	struct edge_t {
		uint32_t from;
		uint32_t label;
		uint32_t to;
		uint32_t id;
	};
	typedef std::vector<const struct edge_t *> Edges;

	struct pre_dfs_t {
		uint32_t id;	
		const struct edge_t *edge;
		const struct pre_dfs_t *prev;
	};
	typedef std::vector<struct pre_dfs_t> Projection;

	struct dfs_code_t {
		uint32_t from;
		uint32_t to;
		int32_t from_label;
		uint32_t edge_label;
		int32_t to_label;

		bool operator != (const struct dfs_code_t& t) const {
			return (this->from != t.from) || (this->to != t.to) || 
				(this->from_label != t.from_label) || (this->edge_label != t.edge_label) || 
				(this->to_label != t.to_label);
		}
	};
	typedef std::vector<struct dfs_code_t> DfsCodes;

	struct dfs_code_project_compare_t {
		bool operator() (const struct dfs_code_t& first,
				const struct dfs_code_t& second) {
			if (first.from_label != second.from_label) {
				return first.from_label < second.from_label;
			} else {
				if (first.edge_label != second.edge_label) {
					return first.edge_label < second.edge_label;
				} else {
					return first.to_label < second.to_label;
				}
			}
		}
	};

	struct dfs_code_backward_compare_t {
		bool operator() (const struct dfs_code_t& first,
				const struct dfs_code_t& second) {
			if (first.to != second.to) {
				return first.to < second.to;
			} else {
				return first.edge_label < second.edge_label;
			}
		}
	};

	struct dfs_code_forward_compare_t {
		bool operator() (const struct dfs_code_t& first,
				const struct dfs_code_t& second) {
			if (first.from != second.from) {
				return first.from > second.from;
			} else {
				if (first.edge_label != second.edge_label) {
					return first.edge_label < second.edge_label;
				} else {
					return first.to_label < second.to_label;
				}
			}
		}
	};

	struct history_t {
		std::vector<uint32_t> edge_labels;
		std::vector<uint32_t> vertex_labels;
	};

	struct vertex_t {
		uint32_t id;		
		int32_t label;
		std::vector<struct edge_t> edges;
	};
	typedef std::vector<struct vertex_t> Vertice;

	class Graph {
		public:
			Graph() : id(0), _m_nedges(0) {};

			size_t size() const
			{
				return _m_vertice.size();
			}

			void resize(size_t s)
			{
				_m_vertice.resize(s);
			}

			void set_id(uint32_t id) 
			{
				this->id = id;
			}

			uint32_t get_id() const
			{
				return id;
			}

			void set_nedges(uint32_t size)
			{
				_m_nedges = size;
			}

			uint32_t get_nedges() const
			{
				return _m_nedges;
			}

			void set_vertice(const Vertice& vertice) 
			{
				this->_m_vertice = vertice;
			}

			struct vertex_t& get_vertex(size_t idx) {
				return _m_vertice[idx];
			}

			const struct vertex_t& get_vertex(size_t idx) const {
				return _m_vertice[idx];
			}

			void clear()
			{
				id = 0;
				_m_vertice.clear();
			}

		private:
			uint32_t _m_nedges;
			uint32_t id;
			Vertice _m_vertice;
	};
}

#endif
