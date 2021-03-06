#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "params.h"
#include "sector.h"
#include "simulation.h"

void run_tests(); // defined in tests.c

static bool uses_time = false;
static bool uses_events = false;

static void set_default_params(){
	sim_data.sector_dims[0] = 1;
	sim_data.sector_dims[1] = 1;
	sim_data.sector_dims[2] = 1;
	sim_data.time_limit = 0.0;
	sim_data.event_limit = 0;
	initial_state_file = NULL;
	final_state_file = NULL;
	compare_file = NULL;
	output_file = NULL;
}

static void check_slice_arg(int slice, char axis){
	if(slice <= 0){
		printf("Error: %c axis needs at least 1 slice.\n", axis);
		exit(1);
	}
}

static void print_config(){
	printf("Number of x slices: %d\n", sim_data.sector_dims[0]);
	printf("Number of y slices: %d\n", sim_data.sector_dims[1]);
	printf("Number of z slices: %d\n", sim_data.sector_dims[2]);
	printf("Time limit: %.17g\n", sim_data.time_limit);
	printf("Event limit: %d\n", sim_data.event_limit);
	printf("Initial state file: %s\n", initial_state_file);
	printf("Output file: %s\n", output_file);
	if(final_state_file != NULL){
		printf("Final state file: %s\n", final_state_file);
	} else {
		printf("Final state file not set.\n");
	}
	if(compare_file != NULL){
		printf("Compare file set to: %s\n", compare_file);
	} else {
		printf("Compare file not set\n");
	}
}

static void validate_args(){
	check_slice_arg(sim_data.sector_dims[X_AXIS], 'x');
	check_slice_arg(sim_data.sector_dims[Y_AXIS], 'y');
	check_slice_arg(sim_data.sector_dims[Z_AXIS], 'z');
	if((uses_time && uses_events) || (!uses_time && !uses_events)){
		printf("Error: either time limit or event limit should be set.\n");
		exit(1);
	}
	if(uses_time && sim_data.time_limit <= 0.0){
		printf("Error: time limit should be > 0\n");
		exit(1);
	}
	if(uses_events && sim_data.event_limit <= 0){
		printf("Error: event limit should be > 0\n");
		exit(1);
	}
	if(output_file == NULL){
		printf("Error: output file (-o) cannot be null\n");
		exit(1);
	}
	if(initial_state_file == NULL){
		printf("Error: initial state file (-i) cannot be null\n");
		exit(1);
	}
}
	
static void print_help(){
	printf("-x, -y, -z:\n");
	printf("\tOptional.\n\tSets the number of slices in the specified axis.\n\tDefaults to 1.\n\tMust be at least 1.\n");
	printf("-c:\n\tOptional.\n\tSets the compare file. This should be a final state file from a previous run.\n\tThe max error between this run and the compare file will be printed.\n");
	printf("-f:\n\tOptional.\n\tSets the final state file. This will contain only the final velocity and position of each sphere.\n");
	printf("-o:\n\tRequired.\n\tSets the output file. This contains all data needed to make use of the simulation.\n");
	printf("-i:\n\tRequired.\n\tSets the initial state file.\n");
	printf("-l:\n\tOptional, but -e is required if -l is unused.\n\tSets the time limit the simulation will run for.\n");
	printf("-e:\n\tOptional, but -l is required if -e is unused.\n\tSets the event limit the simulation will run for.\n");
	printf("-t:\n\tOptional.\n\tRuns some tests which verify the collision system works.\t\nIf set then all other work is skipped and other args are ignored.\n");
	exit(0);
}

void parse_args(int argc, char *argv[]) {
	set_default_params();
	int c;
	while((c = getopt(argc, argv, "i:c:f:ho:x:y:z:l:te:")) != -1) {
		switch(c) {
		case 'x':
			sim_data.sector_dims[X_AXIS] = atoi(optarg);
			break;
		case 'y':
			sim_data.sector_dims[Y_AXIS] = atoi(optarg);
			break;
		case 'z':
			sim_data.sector_dims[Z_AXIS] = atoi(optarg);
			break;
		case 'f':
			final_state_file = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		case 'c':
			compare_file = optarg;
			break;
		case 'h':
			print_help();
			break;
		case 'i':
			initial_state_file = optarg;
			break;
		case 'l':
			sim_data.time_limit = atof(optarg);
			sim_data.uses_time_limit = true;
			uses_time = true;
			break;
		case 't':
			run_tests();
			exit(0);
			break;
		case 'e':
			sim_data.event_limit = atoi(optarg);
			sim_data.uses_time_limit = false;
			uses_events = true;
			break;
		}
	}
	validate_args();
	print_config();
}
