#include <pthread.h>
#include "gspan.h"

namespace gspan {
	bool GSpan::split(ProjectionMap& projection_map)
	{
		//the size is not the actual size of the projection_map, but the
		//projection map which contains itemss greater than support
		size_t map_size = 0;

		for (ProjectionMap::reverse_iterator it = projection_map.rbegin(); it != projection_map.rend(); ++it) {
			if ((it->second).size() < _m_nsupport)
				continue;
			++map_size;
		}

		//can not be divided 
		if (map_size < THREAD_NUM)
			return false;

		size_t interval = map_size / THREAD_NUM;
		size_t tid = 0;
		size_t idx_output = 0;

		
		for (ProjectionMap::reverse_iterator it = projection_map.rbegin(); it != projection_map.rend(); ++it) {
			if ((it->second).size() < _m_nsupport)
				continue;

			if (idx_output == interval * tid) {
				_m_output[tid] = new Output(_m_output_name, tid);
				_m_split_idx[tid] = it;
				++tid;
			}
			++idx_output;

			if (tid == THREAD_NUM)
				break;
		}
		_m_split_idx[tid] = projection_map.rend();

		return true;
	}	

	void GSpan::multi_subgraph_init()
	{
		//the posix standard
		pthread_t thread[THREAD_NUM];
		int ret_code = 0;

		//set joinable
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		struct thread_t threads_entry[THREAD_NUM];

		for (size_t i = 0; i < THREAD_NUM; ++i) {
			threads_entry[i].gspan_entry = this;
			threads_entry[i].tid = i;

			ret_code = pthread_create(&thread[i], &attr, internal_thread_func, (void *)&threads_entry[i]);
			if (ret_code) {
				fprintf(stderr, "create thread error! %d\n", ret_code);
				exit(GSPAN_ERROR);
			}
		}

		pthread_attr_destroy(&attr);
		for (size_t i = 0; i < THREAD_NUM; ++i) {
			ret_code = pthread_join(thread[i], NULL);
			if (ret_code) {
				fprintf(stderr, "destory thread error! %d\n", ret_code);
				exit(GSPAN_ERROR);
			}
		}

		return;
	}

	void *GSpan::internal_thread_func(void *para) 
	{
		struct thread_t *p_thread = static_cast<struct thread_t*>(para);

		(p_thread->gspan_entry)->multi_subgraph_mining(p_thread->tid);
	}	

	void* GSpan::multi_subgraph_mining(size_t tid)
	{
		int32_t prev_id = -1;
		for (ProjectionMap::reverse_iterator it = _m_split_idx[tid]; it != _m_split_idx[tid + 1]; ++it) {
			//another parital pruneing, like apriori
			if ((it->second).size() < _m_nsupport)
				continue;

			_m_dfs_codes[tid].push_back((struct dfs_code_t)
					{0, 1, (it->first).from_label, (it->first).edge_label, (it->first).to_label});

			subgraph_mining(it->second, tid, prev_id);
			prev_id = -1;

			_m_dfs_codes[tid].pop_back();		
		}

		pthread_exit(NULL);
	}
}
