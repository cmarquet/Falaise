// snemo/digitization/tracker_zone.cc
// Author(s): Yves LEMIERE <lemiere@lpccaen.in2p3.fr>
// Author(s): Guillaume OLIVIERO <goliviero@lpccaen.in2p3.fr>

// Standard library : 
#include <iostream>

// Ourselves:
#include <falaise/snemo/digitization/tracker_zone.h>

namespace snemo {

  namespace digitization {

    tracker_zone::tracker_zone()
    {
      reset();
      return;
    }

    void tracker_zone::reset()
    {
      for (int ilayer = 0; ilayer < snemo::digitization::tracker_info::NLAYERS; ilayer++) {
	for (int irow = 0; irow < snemo::digitization::tracker_info::ZONE_WIDTH; irow++) {
	  _cells_[ilayer][irow] = false;
	}
      }
      return;
    }

    int tracker_zone::start_row(int i_)
    {
      static const int start_row[snemo::digitization::tracker_info::NZONES]
	= {0, 9, 21, 33, 45, 57, 68, 80, 92, 104};
      return start_row[i_];
    }
    
    int tracker_zone::stop_row(int i_)
    {
      static const int stop_row[snemo::digitization::tracker_info::NZONES]
	= {8, 20, 32, 44, 56, 67, 79, 91, 103, 112};
      return stop_row[i_];
    }

    int tracker_zone::width(int i_)
    {
      return stop_row(i_) - start_row(i_) + 1;
    }


    void tracker_zone::print_layout(std::ostream & out_)
    {
      out_ << "Zone layout: " << '\n';
      out_ << "#Zone   Rows    #\n";
      for (int izone = 0; izone < snemo::digitization::tracker_info::NZONES; izone++) {
	out_ << izone << " " << start_row(izone) << "-" << stop_row(izone)
	     << ' ' << "#" << width(izone) << '\n';
      }
      out_ << std::endl;
      return;
    }


  } // end of namespace digitization

} // end of namespace snemo