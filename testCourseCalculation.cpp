#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "CourseCalculation.h"
#include <iostream>

TEST_CASE("CourseCalculationTest")
{
	SECTION("Test CTS is BTW unless course is within TACK_ANGLE degrees from TWD, if not tack")
	{
		CourseCalculation cc;
		int arrSize = 6;
		double boatLats [] 		= {60.836881, 60.073063, 60.073063, 59.539888, 59.539888, 60.073063 };
		double boatLongs [] 		= {19.143219, 22.185974, 22.185974, 19.894409, 19.894409, 22.185974};
		double waypointLats [] 	= {60.103333, 60.103172, 60.103172, 60.103362, 60.103362, 60.103172};
		double waypointLongs [] 	= {19.928306, 19.92834, 19.92834, 19.928601, 19.928601, 19.92834};

		const int tack_angle = 45;
		const int sector_angle = 5;
		const int TWD = 270;

		cc.setTACK_ANGLE(tack_angle);
		cc.setSECTOR_ANGLE(sector_angle);
		cc.setTWD(TWD);

		const double min_within_value = cc.getTWD() - tack_angle;
		const double max_within_value = cc.getTWD() + tack_angle;


		for(int i = 0; i < arrSize; i++) {
			cc.calculateBTW(boatLats[i], boatLongs[i], waypointLats[i], waypointLongs[i]);
			cc.calculateDTW(boatLats[i], boatLongs[i], waypointLats[i], waypointLongs[i]);
			
			cc.calculateCTS();


			/* If desired course is within TACK_ANGLE +/- TWD then we should be tacking.*/
			if(cc.getCTS() >= min_within_value && cc.getCTS() <= max_within_value) {
				/* Now we should be tacking. */
				REQUIRE(cc.getTACK());

				/* Course should now be equal to TWD +/- TACK_ANGLE. */
				bool course_OK = false;

				if(cc.getCTS() == min_within_value || cc.getCTS() == max_within_value) {
					course_OK = true;	
				}
				
				REQUIRE(course_OK);						
			}
			else {
				/* courseToSteer should be equal to BearingToWayPoint.
				   And we should NOT be tacking. */
				REQUIRE(cc.getCTS() == cc.getBTW());
				REQUIRE_FALSE(cc.getTACK());
			}				
		}
	}


	SECTION("Test calculateDTW with 3 different positions for waypoint and boat")
	{
		const int noOfDTW = 3;
		double boatLats [3] 		= {60.836881, 60.073063, 59.539888 };
		double boatLongs [3] 		= {19.143219, 22.185974, 19.894409 };
		double waypointLats [3] 	= {60.103333, 60.103172, 60.103362 };
		double waypointLongs [3] 	= {19.928306, 19.92834, 19.928601 };

		/* Resulting distance between boat and waypont in meters.
		 * Taken from http://www.movable-type.co.uk/scripts/latlong.html 
		 */
		const double result_distance[3] = { 92218.1699654549, 125222.8858237675, 62684.5898278684 };

		CourseCalculation cc;
		for(int i = 0; i < noOfDTW; i++) {
			cc.calculateDTW(boatLats[i], boatLongs[i], waypointLats[i], waypointLongs[i]);
			REQUIRE(result_distance[i] == Approx(cc.getDTW()));
		}
	}

	SECTION("Test calculateBTW with 3 different positions for waypoint and boat")
	{
		const int noOfBTW = 3;
		double boatLats [3] 		= {60.836881, 60.073063, 59.539888 };
		double boatLongs [3] 		= {19.143219, 22.185974, 19.894409 };
		double waypointLats [3] 	= {60.103333, 60.103172, 60.103362 };
		double waypointLongs [3] 	= {19.928306, 19.92834, 19.928601 };

		/* Resulting bearing between boat and waypont.
		 * Taken from http://www.movable-type.co.uk/scripts/latlong.html 
		 */
		const double result_bearing[3] = { 151.845945039753, 272.51025491197, 1.732425848657 };

		CourseCalculation cc;
		for(int i = 0; i < noOfBTW; i++) {
			cc.calculateBTW(boatLats[i], boatLongs[i], waypointLats[i], waypointLongs[i]);
			REQUIRE(result_bearing[i] == Approx(cc.getBTW()));
		}
	}
}





















