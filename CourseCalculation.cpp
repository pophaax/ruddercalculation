#include "CourseCalculation.h"

const double PI = 3.1415926;
const double radiusOfEarth = 6378.1;

CourseCalculation::CourseCalculation() {
	m_PREVIOUS_ITERATION_TACK = false;
}

CourseCalculation::~CourseCalculation() {
}

void CourseCalculation::radianConversion(double boatLat, double boatLong,
		double wpLat, double wpLong) {

	m_deltaLatitudeRadians = (wpLat - boatLat) * PI / 180;
	m_deltaLongitudeRadians = (wpLong - boatLong) * PI / 180;
	m_latitudeRadians = boatLat * PI / 180;
	m_wpLatitudeRadians = wpLat * PI / 180;
	//m_longitudeRadians = boatLat * PI / 180;
	//m_wpLongitudeRadians = wpLong * PI / 180;
}

void CourseCalculation::calculateBWP(double boatLat, double boatLong,
		double wpLat, double wpLong) {

	radianConversion(boatLat, boatLong, wpLat, wpLong);

	double y = sin(m_deltaLongitudeRadians) * cos(m_wpLatitudeRadians);
	double x = cos(m_latitudeRadians) * sin(m_wpLatitudeRadians)
			- sin(m_latitudeRadians) * cos(m_wpLatitudeRadians)
					* cos(m_deltaLongitudeRadians);

	double bwp = atan2(y, x) / PI * 180;

	if (bwp < 0) {

		bwp = 360 + bwp;
	}

	this->m_BWP = bwp;
}

void CourseCalculation::calculateDWP(double boatLat, double boatLong,
		double wpLat, double wpLong) {

	radianConversion(boatLat, boatLong, wpLat, wpLong);

	double a = sin(m_deltaLatitudeRadians / 2) * sin(m_deltaLatitudeRadians / 2)
			+ sin(m_deltaLongitudeRadians / 2)
					* sin(m_deltaLongitudeRadians / 2) * cos(m_latitudeRadians)
					* cos(m_wpLatitudeRadians);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	double d = radiusOfEarth * c;
	d = d * 1000;

	this->m_DWP = d;
}

int CourseCalculation::determineFirstCTS() {

	int cts;
	int port = countDown();
	int starboard = countUp();

	if (port > starboard) {
		cts = calculatePortCTS();
		m_GOING_STARBOARD = false;
	}

	else if (starboard >= port) {
		cts = calculateStarboardCTS();
		m_GOING_STARBOARD = true;
	}

	return cts;
}

bool CourseCalculation::continuePort() {

	int tmp_BWP = round(m_BWP);
	int tmp_TWD = m_TWD;
	bool continueDirection = false;
	int sectorBegin = tmp_TWD - m_TACK_ANGLE;

	if (sectorBegin < 0) {

		sectorBegin += 360;
	}

	for (int i = 0; i < (m_TACK_ANGLE + m_SECTOR_ANGLE); i++) {

		if (tmp_BWP == sectorBegin) {

			continueDirection = true;
			break;
		}

		sectorBegin++;

		if (sectorBegin == 360) {

			sectorBegin = 0;
		}
	}

	return continueDirection;
}

bool CourseCalculation::continueStarboard() {

	int tmp_BWP = round(m_BWP);
	int tmp_TWD = m_TWD;
	bool continueDirection = false;
	int sectorBegin = tmp_TWD + m_TACK_ANGLE;

	if (sectorBegin >= 360) {

		sectorBegin -= 360;
	}

	for (int i = 0; i < (m_TACK_ANGLE + m_SECTOR_ANGLE); i++) {

		if (tmp_BWP == sectorBegin) {
			continueDirection = true;
			break;
		}

		sectorBegin--;

		if (sectorBegin == -1) {

			sectorBegin = 359;
		}
	}

	return continueDirection;
}

double CourseCalculation::calculateStarboardCTS() {

	double cts;

	if (m_TWD + m_TACK_ANGLE > 360) {
		cts = m_TWD + m_TACK_ANGLE - 360;
	} else {
		cts = m_TWD + m_TACK_ANGLE;
	}

	return cts;
}

double CourseCalculation::calculatePortCTS() {

	double cts;

	if (m_TWD - m_TACK_ANGLE < 0) {
		cts = m_TWD - m_TACK_ANGLE + 360;
	} else {
		cts = m_TWD - m_TACK_ANGLE;
	}
	return cts;
}

void CourseCalculation::calculateCTS() {

	double cts;

	calculateTACK();

	if (m_TACK == true) {
		// We are going against the wind direction
		if (m_PREVIOUS_ITERATION_TACK == false) {
			// We just ended up in tack position
			cts = determineFirstCTS();
		}

		else {
			// We are still in tack position
			if (m_GOING_STARBOARD) {
				// Wind is coming from port but we are going in starboard direction relative to the wind
				if (!continueStarboard()) {
					// CTS should switch to port side
					m_GOING_STARBOARD = false;
					cts = calculatePortCTS();
				}

				else {
					// CTS should remain about the same
					cts = calculateStarboardCTS();
				}
			}

			else if (!m_GOING_STARBOARD) {
				// Wind is coming from starboard but we are going in port direction relative to the wind
				if (!continuePort()) {
					// CTS should switch to starboard side
					m_GOING_STARBOARD = true;
					cts = calculateStarboardCTS();
				}

				else {
					// CTS should remain about the same
					cts = calculatePortCTS();
				}
			}
		}
	}

	else {
		// We are not going against the wind direction
		cts = m_BWP;
	}

	m_CTS = cts;
	m_PREVIOUS_ITERATION_TACK = m_TACK;

}

int CourseCalculation::countUp() {

	int count = 0;
	int tmp_BWP = round(m_BWP);
	int tmp_TWD = m_TWD;
	bool go = true;

	for (int i = 0; i < m_TACK_ANGLE && go == true; i++) {

		if (tmp_TWD == tmp_BWP) {
			go = false;
		}

		count++;
		tmp_TWD++;

		if (tmp_TWD == 360) {

			tmp_TWD = 0;
		}
	}

	return count;
}

int CourseCalculation::countDown() {

	int count = 0;
	int tmp_BWP = round(m_BWP);
	int tmp_TWD = m_TWD;
	bool go = true;

	for (int i = 0; i < m_TACK_ANGLE && go == true; i++) {

		if (tmp_TWD == tmp_BWP) {
			go = false;
		}

		count++;
		tmp_TWD--;

		if (tmp_TWD == -1) {

			tmp_TWD = 359;
		}
	}

	return count;
}

bool CourseCalculation::calcUp() {

	int tmp_TWD = m_TWD;
	int tmp_BWP = round(m_BWP);

	bool tack = false;

	for (int i = 0; i < m_TACK_ANGLE; i++) {

		if (tmp_TWD == tmp_BWP) {
			tack = true;
		}

		tmp_TWD++;

		if (tmp_TWD == 360) {
			tmp_TWD = 0;
		}

	}

	return tack;
}

bool CourseCalculation::calcDown() {

	int tmp_TWD = m_TWD;
	int tmp_BWP = round(m_BWP);
	bool tack = false;

	for (int i = 0; i < m_TACK_ANGLE; i++) {

		if (tmp_TWD == tmp_BWP) {
			tack = true;
		}

		tmp_TWD--;

		if (tmp_TWD == -1) {
			tmp_TWD = 359;
		}

	}

	return tack;
}

bool CourseCalculation::calculateTACK() {

	if (calcUp() || calcDown()) {
		m_TACK = true;
	}

	else {
		m_TACK = false;
	}

	return m_TACK;
}

void CourseCalculation::setTWD(double degrees) {
	this->m_TWD = degrees;
}

void CourseCalculation::setBWP(double degrees) {
	this->m_BWP = degrees;
}

void CourseCalculation::setTACK_ANGLE(double degrees) {
	this->m_TACK_ANGLE = degrees;
}

void CourseCalculation::setSECTOR_ANGLE(double degrees) {
	this->m_SECTOR_ANGLE = degrees;
}

double CourseCalculation::getCTS() {

	return m_CTS;
}

double CourseCalculation::getBWP() {

	return m_BWP;
}

double CourseCalculation::getDWP() {

	return m_DWP;
}

double CourseCalculation::getTWD() {

	return m_TWD;
}

bool CourseCalculation::getTACK() {

	return m_TACK;
}