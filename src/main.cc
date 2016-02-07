#include "sys/time.h"
#include "gspan.h"
#include "database.h"
#include "gspan_utils.h"

//initialize instance
using gspan::DataBase;
DataBase* DataBase::_m_instance = new DataBase();

int main(int argc, char *argv[])
{
	gspan::parameters_t parameters;
	
	//gspan -file ../data/ -support 0.30
	uint32_t nargh = gspan::get_parameters(&parameters, argc, argv);

	//[parameters] - [number of default parameters]
	if (nargh != gspan::get_narg() - 1) {
		exit(GSPAN_ERROR);
	}

	std::string seperator;
	gspan::get_seperator(&parameters, seperator);
	gspan::print_parameters(&parameters);

	timeval t1, t2;
	double elapsed_time = 0.0f;
	gettimeofday(&t1, NULL);

	gspan::GSpan gspan;
	gspan.execute(seperator.c_str(), parameters.file_path, parameters.output_name, parameters.support);	

	gettimeofday(&t2, NULL);
	elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0; 
	elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0;
	printf("elapsed time %f\n", elapsed_time);
	return 0;
}
