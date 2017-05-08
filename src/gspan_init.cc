#include "sys/time.h"
#include "gspan.h"
#include "seperator.h"
#include "database.h"

namespace gspan {

	void GSpan::execute(const char *seperator_type, const char *file_path, const char *output_name, double support) 
	{
		_m_seperator = new Seperator(seperator_type);
		_m_file_path = file_path;
		_m_output_name = output_name;
		_m_support = support;

		Input gspan_input;				

		_m_seperator->seperate(_m_file_path, gspan_input);

		timeval t1, t2;
		double elapsed_time = 0.0f;
		gettimeofday(&t1, NULL);

#ifdef DEBUG
		printf("seperate\n");
#endif
		if (GSPAN_SUCCESS != read_input(gspan_input)) {
			fprintf(stderr, "read input error!");
			exit(GSPAN_ERROR);
		}

#ifdef DEBUG
		printf("find_frequent\n");
#endif

		if (GSPAN_SUCCESS != find_frequent_nodes()) {
			fprintf(stderr, "find frequent nodes error!");
			exit(GSPAN_ERROR);
		}	

		if (GSPAN_SUCCESS != reconstruct(gspan_input)) {
			fprintf(stderr, "find frequent nodes error!");
			exit(GSPAN_ERROR);
		}	

		DataBase *database = DataBase::get_instance();
		_m_graphs = database->get_graph();

#ifdef DEBUG
		printf("project\n");
#endif

		if (GSPAN_SUCCESS != project()) {
			fprintf(stderr, "projection nodes error!");
			exit(GSPAN_ERROR);
		}	

		gettimeofday(&t2, NULL);
		elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0; 
		elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
		printf("elapsed time->execute %f\n", elapsed_time);
	}

	GSpanReturnCode GSpan::read_input(Input& input) 
	{
		Graph graph;
		Vertice vertice;

		DataBase *database = DataBase::get_instance();

		uint32_t graph_idx = 0;
		uint32_t edge_id = 0;
		double total_edge = 0.0f;
		double density = 0.0f;
		std::vector<std::set<uint32_t> > edge_label_set;

		for (size_t i = 0; i < input.size(); ++i) {
			if (input[i][0] == "t") {
				if (i != 0) {
					total_edge += edge_id;
					if (vertice.size() != 0) {
						density += (2.0f * edge_id) / (vertice.size() * (vertice.size() - 1));
					}
					graph.set_nedges(edge_id);
					graph.set_vertice(vertice);
					edge_id = 0;
					database->push_graph(graph);	
					graph.clear();
					vertice.clear();
				}

				edge_label_set.resize(edge_label_set.size() + 1);

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

				//add label
				edge_label_set[graph_idx - 1].insert(label);

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
		total_edge += edge_id;
		if (vertice.size() != 0) {
			density += (2.0f * edge_id) / (vertice.size() * (vertice.size() - 1));
		}
		
		printf("average graph size : %f\n", total_edge / database->size());
		printf("average density : %f\n", density / database->size());
		std::vector<std::set<uint32_t> > unique_set(graph_idx * (graph_idx + 1) / 2);

		for (size_t i = 0; i < graph_idx; ++i) {
			unique_set[i] = edge_label_set[i];
			for (size_t j = 0; j < graph_idx; ++j) {
				if (unique_set[i].size() == 0) 
					break;

				if (j == i) 
					continue;

				for (std::set<uint32_t>::iterator edge_iterator = edge_label_set[j].begin(); 
						edge_iterator != edge_label_set[j].end(); ++edge_iterator) {
					unique_set[i].erase(*edge_iterator);
				}
			}
		}

		for (size_t i = 0; i < graph_idx; ++i) {
			printf("unique edge label set of %zu : \n", i);
			if (unique_set[i].size() != 0) {
				for (std::set<uint32_t>::iterator label_iterator = unique_set[i].begin(); 
						label_iterator != unique_set[i].end(); ++label_iterator) {
					printf("%u ", *label_iterator);
				}
				printf("\n");
			}
		}
		
		_m_nsupport = static_cast<uint32_t>(graph_idx * _m_support);

		return GSPAN_SUCCESS;
	}
	
}//namespace gspan


