
read_input()
{
	for (size_t i = 0;	
}

find_frequent_vertex()
{
	for (size_t i = 0; i < labels.size(); i++) {
		if (!(labels[i] < min_support)) 
			continue;

		uint32_t label = labels[i];
		for (size_t j = 0; j < graph.size(); j++) {
			for (size_t k = 0; k < graph[j].size(); j++) {
				if (graph[j][k].label == label) {
					graph_labels[label].push_back(graph_id, vertex_id);
				}
			}
		}
	}

}

construct_frequent_two()
{
	for (size_t i = 0; i < graph_labels.size(); i++) {
		if (graph_labels[i] >= _m_min_support) {
			output.push_back();
			//
			//construct root
			//
		}
	}
}

project_two
{
	// struct compare_t {
	// 	int32_t from_label;
	// 	int32_t edge_label;
	// 	int32_t to_label;
	// };
	std::map<struct compare_t, struct prev_dfs_t> compare;
	for (size_t i = 0; i < graph.size(); i++) {
		for (size_t j = 0; j < graph[i].vertice.size(); ++j) {
			if (frequent_labels[graph[i][j].labels]) {
				struct edge_t edges;
				get_forward_edges(graph[i], edges);
				compare.push_back(
				
			}
		}	
	}


	for (std::map<struct compare_t, struct prev_dfs_t>::iterator it = compare.begin();
			it != compare.end(); ++it) {
		dfscode.push_back(it->first);	
		subgraph_mining(it->second);
		dfscode.pop_back();
	}


}

mark_edges()
{
//	
//	struct compare_t {
//	 uint32_t from, unint32_t to, uint32_t label; 
//	}
	std::map<struct compare_t, uint32_t> mp
//	if (from == t.from && to == t.to && label = t. label ||
//			from == t.to && to == t.from && label = t.label) {
//		return 1;
//	}
	
	for (size_t i = 0; i < graph.size(); i++) {
		for (size_t j = 0;  j < edge.size(); j++) {
			if (!mp[from, to, label]) {
				++id;
				mp[from, to, label] = id;
				edge->id = id;
			} else {
				edge->id = mp[from, to, label];
			}
		}	
	}	
	
}

generate_graph(dfs_code)
{
	g.resize(dfs_code.maxlabel);	

	for (size_t i = 0; i < dfs_code.size(); i++) {
		if (it->from_label != -1) {
			g[it->from].label = it->from_label;
		}

		if (it->to_label != -1) {
			g[it->to].label = it->to_label;
		}

		g[it->from].push_back(from, to ,label);
		g[it->to].push_back(to, from, label);
	}

	mark_edges();
}

is_min()
{
	if (dfs_codes.size() == 1) 
		return true;	

	generate_graph(dfs_code);

	project the graphs

	start another projection
}

subgraph_min_detect()
{	
	build_right_most_path();

	enumerate();

	project_backward();

	enumerate();

	project_forward();
}

count_support(project)
{
	uint32_t support = 0;
	uint32_t prev_id = 0;

	for (size_t i = 0; i < project.size(); ++i) {
		if (i == 0 || prev_id != project[i].id) {
			prev_id = project[i].id;
			++support;
		}
	}

	return support;
}

std::vector<uint32_t> build_right_most_path(right_most_path)
{
	std::vector<uint32_t> right_most_path;
	uint32_t prev_from = -1;

	for (size_t i = dfscode.size() - 1; i >= 0; --i) {
		if (dfscode[i].from < dfscode[i].to) {
			if (!right_most_path.size() || prev_from == dfscode[i].to)  {
				right_most_path.push_back(i);
				prev_from = dfscode[i].from;
			}
		}
	}
}

get_history(history)
{
	push_back(e->edge);
	edges[e->edge->edge_label] = 
		vertice[e->edge->from_id] = 
		vertice[e->edge->to_id] = true;

	for (struct prev_dfs_t *p = e->prev; p != NULL; p = p->prev) {
		history.push_back(p->edge);

		edges[e->edge->edge_label] = 
			vertice[e->edge->from_id] = 
			vertice[e->edge->to_id] = true;
	}

	std::reverse(history);
}

get_backward()
{
	//the less the from edge id, the first their occurrence
	backward_edges;
	for (size_t rm_idx = right_most_path.size() - 1; rm_idx >= 1; --rm_idx) {
		if (e1 == e2) //can it happens? why? test it
			return 0;
		

		for (size_t i = 0; i < graph[e2->to].edge.size(); ++) {
			if (!history.has_edge(it->id) {
				continue;
			}
			
			if (it->to == e1->from) {
				if (e1->elabel < it->elabel || 
					(e1->elabel == it->elabel && graph[e1->to].label <= graph[e2->to].label)) {
					backward_edges.push_bakc(*it);
				}
			}
		}
	}

}

get_first_forward()
{
	for (Vertex::edge_iterator it = graph[e->to].edge.begin() ;
		it != graph[e->to].edge.end() ; ++it)
	{
		if (minlabel > graph[it->to].label || history.hasVertex (it->to))//minlabel why?test it?
			continue;

		result.push_back (&(*it));
	}

}

get_other_forward()
{
	for (size_t i = 0; i < right_most_path.size(); i++) {
	for (Vertex::edge_iterator it = graph[e->from].edge.begin() ;
		it != graph[e->from].edge.end() ; ++it)
	{
		int tolabel2 = graph[it->to].label;
		if (e->to == it->to || minlabel > tolabel2 || history.hasVertex (it->to))
			continue;

		if (e->elabel < it->elabel || (e->elabel == it->elabel && tolabel <= tolabel2))
			result.push_back (&(*it));
	}
	}

}

enumerate();
{
	for (size_t graph_id = 0; graph_id < projected.size(); ++graph_id) {
		struct History history;
		bulid_history(history, graph_id, prev_dfs);
		
		//get_backward which can only grow from the right most vertex
		edges = get_backward();	
		//
		//get_firstforward
		get_first_forward();
		//
		//get_otherforward
		get_other_forward();
	}
}

subgraph_mining()
{
	count_support();

	if (!is_min())
		return ;

	output.push_back();

	build_right_most_path();

	enumerate();

	project_backward();

	project_forward();
}


