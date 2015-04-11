//test_signal_to_tp_process.cxx

// Standard libraries :
#include <iostream>

// GSL:
#include <bayeux/mygsl/rng.h>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/sd_to_geiger_signal_algo.h>
#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/signal_to_geiger_tp_algo.h>

int main( int /* argc_ */, char ** /* argv_ */ )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  try {
    std::clog << "Test program for class 'snemo::digitization::sd_to_calo_tp_algo' !" << std::endl;
    int32_t seed = 314159;
    std::string manager_config_file;
    
    manager_config_file = "~/data/my_falaise/config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env (manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
   
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_manager.initialize (manager_config);

    std::string pipeline_simulated_data_filename;
    std::string SD_bank_label = "SD";

    pipeline_simulated_data_filename = "/home/guillaume/data/my_falaise/outputs/Se82_0nubb-source_strips_bulk_SD.brio";
    
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", 1);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    datatools::things ER;
    snemo::digitization::ID_convertor my_convertor;
    my_convertor.set_geo_manager(my_manager);
    my_convertor.set_module_number(0);
    my_convertor.initialize();

    mygsl::rng random_generator;
    random_generator.initialize(seed);
    int32_t clocktick_reference_25 = random_generator.uniform_int(25);
    double clocktick_shift_25 = random_generator.flat(0.0, 25.0 * CLHEP::nanosecond);

    snemo::digitization::sd_to_geiger_signal_algo sd_2_geiger_signal(my_manager);
    sd_2_geiger_signal.initialize();

    snemo::digitization::sd_to_calo_signal_algo sd_2_calo_signal(my_manager);
    sd_2_calo_signal.initialize();

    int psd_count = 0;
    while (!reader.is_terminated())
      {
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);

	    int32_t clocktick_reference_25 = random_generator.uniform_int(25);
	    double clocktick_shift_25 = random_generator.flat(0.0, 25.0 * CLHEP::nanosecond);

	    snemo::digitization::signal_to_geiger_tp_algo signal_2_geiger_tp;
	    signal_2_geiger_tp.initialize(my_convertor);
	    signal_2_geiger_tp.set_clocktick_reference(clocktick_reference_25 * 32);
	    signal_2_geiger_tp.set_clocktick_shift(clocktick_shift_25);	    

	    snemo::digitization::signal_to_calo_tp_algo signal_2_calo_tp;
	    signal_2_calo_tp.initialize(clocktick_reference_25, clocktick_shift_25, my_convertor);

	    snemo::digitization::signal_data signal_data;
	    if( SD.has_step_hits("gg"))
	      {		  
		sd_2_geiger_signal.process(SD, signal_data);
	      }
	    if( SD.has_step_hits("calo"))
	      {		  
		sd_2_calo_signal.process(SD, signal_data);
	      }
	    signal_data.tree_dump(std::clog, "Signal data : ", "INFO : ");

	    snemo::digitization::geiger_tp_data my_geiger_tp_data;
	    snemo::digitization::calo_tp_data my_calo_tp_data;

	    if( signal_data.has_geiger_signals())
	      {		  
	    	signal_2_geiger_tp.process(signal_data, my_geiger_tp_data);
	    	my_geiger_tp_data.tree_dump(std::clog, "Geiger TP(s) data : ", "INFO : ");
	      }	    

	    if( signal_data.has_calo_signals())
	      {
		signal_2_calo_tp.process(signal_data, my_calo_tp_data);
		my_calo_tp_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");
	      }

	  }     
	// CF README.RST pour display graphique avec loader de manager.conf
	// -> /home/guillaume/data/Bayeux/Bayeux-trunk/source/bxmctools/examples/ex00
	ER.clear();

	psd_count++;
	std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      }
    
    std::clog << "The end." << std::endl;
  }

  catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }

  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  FALAISE_FINI();
  return error_code;
}