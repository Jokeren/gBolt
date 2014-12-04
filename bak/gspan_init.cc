#include "gspan.h"
#include "seperator.h"
#include "database.h"

namespace gspan {

	void GSpan::execute(const char *seperator_type, const char *file_path, double support) 
	{
		_m_seperator = new Seperator(seperator_type);
		_m_file_path = file_path;
		_m_support = support;

		Input gspan_input;				

		_m_seperator->seperate(_m_file_path, gspan_input);

		//debug
		//printf("seperate\n");
		if (GSPAN_SUCCESS != read_input(gspan_input)) {
			fprintf(stderr, "read input error!");
			exit(GSPAN_ERROR);
		}

		//debug
		//printf("find_frequent\n");
		//init graphs using in this turn
		if (GSPAN_SUCCESS != find_frequent_nodes()) {
			fprintf(stderr, "find frequent nodes error!");
			exit(GSPAN_ERROR);
		}	

		if (GSPAN_SUCCESS != reconstruct()) {
			fprintf(stderr, "find frequent nodes error!");
			exit(GSPAN_ERROR);
		}	

		//debug
		//printf("project\n");
		if (GSPAN_SUCCESS != project()) {
			fprintf(stderr, "projection nodes error!");
			exit(GSPAN_ERROR);
		}	
	}

	GSpanReturnCode GSpan::read_input(Input& input) 
	{
		Graph graph;
		Vertice vertice;

		DataBase *database = DataBase::get_instance();

		uint32_t graph_idx = 0;
		uint32_t edge_id = 0;
		for (size_t i = 0; i < input.size(); ++i) {
			if (input[i][0] == "t") {
				if (i != 0) {
					graph.set_nedges(edge_id);
					graph.set_vertice(vertice);
					edge_id = 0;
					database->push_graph(graph);	
					graph.clear();
					vertice.clear();
				}

				char indicator, seperator;
				uint32_t idx;
				indicator = input[i][0][0];
				seperator = input[i][1][0];
				sscanf(input[i][2].c_str(), "%u", &idx);

				if (graph_idx != idx) {
					fprintf(stderr, "reading input warning! %u %u\n", graph_idx, idx);	
				}
				graph.set_id(idx);

				++graph_idx;
			} else if (input[i][0] == "v") {
				char indicator;
				uint32_t id, label;
				indicator = input[i][0][0];
				sscanf(input[i][1].c_str(), "%u", &id);
				sscanf(input[i][2].c_str(), "%u", &label);

				struct vertex_t vertex;
				vertex.id = id;
				vertex.label = label;	

				vertice.push_back(vertex);
			} else if (input[i][0] == "e") {
				char indicator;
				uint32_t from, to, label;
				indicator = input[i][0][0];
				sscanf(input[i][1].c_str(), "%u", &from);
				sscanf(input[i][2].c_str(), "%u", &to);
				sscanf(input[i][3].c_str(), "%u", &label);
				//debug
				//printf("edge label %u\n", label);

				struct edge_t edge;
				edge.from = from;
				edge.to = to;
				edge.label = label;
				edge.id = edge_id;
				++edge_id;

				//first edge
				vertice[from].edges.push_back(edge);

				//second edge
				edge.from = to;
				edge.to = from;
				vertice[to].edges.push_back(edge);
			} else {
				fprintf(stderr, "reading input warning!");	
			}
		}

		graph.set_vertice(vertice);
		database->push_graph(graph);	
		
		_m_nsupport = static_cast<uint32_t>(graph_idx * _m_support);

		return GSPAN_SUCCESS;
	}
	
}//namespace gspan


