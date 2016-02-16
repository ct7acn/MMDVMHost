/*
 *   Copyright (C) 2015,2016 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "TFTSerial.h"
#include "Log.h"

#include <cstdio>
#include <cassert>
#include <cstring>

CTFTSerial::CTFTSerial(const std::string& port, unsigned int brightness) :
m_serial(port, SERIAL_9600),
m_brightness(brightness)
{
	assert(brightness >= 0U && brightness <= 100U);
}

CTFTSerial::~CTFTSerial()
{
}

bool CTFTSerial::open()
{
	bool ret = m_serial.open();
	if (!ret) {
		LogError("Cannot open the port for the TFT Serial");
		return false;
	}

	setRotation(3U);

	setFontSize(1U);

	setBrightness(m_brightness);

	// Set background white
	setBackground(7U);

	// Set foreground black
	setForeground(0U);

	setIdle();

	return true;
}

void CTFTSerial::setIdle()
{
	// Clear the screen
	clearScreen();

	// Draw MMDVM logo
	displayImage(0U, 0U, "MMDVM_sm.bmp");

	// Draw all mode insignias
	displayImage(0U, 30U, "ALL_sm.bmp");
}

void CTFTSerial::setDStar()
{
	// Clear the screen
	clearScreen();

	// Draw MMDVM logo
	displayImage(0U, 0U, "MMDVM_sm.bmp");

	// Draw D-Star insignia
	displayImage(0U, 30U, "DStar_sm.bmp");

	displayText(0U, 8U, "Listening");
}

void CTFTSerial::writeDStar(const char* my1, const char* my2, const char* your)
{
	assert(my1 != NULL);
	assert(my2 != NULL);
	assert(your != NULL);

	char text[20U];
	::sprintf(text, "%8.8s/%4.4s -> %8.8s", my1, my2, your);

	displayText(0U, 8U, text);
}

void CTFTSerial::clearDStar()
{
	// Draw MMDVM logo
	displayImage(0U, 0U, "MMDVM_sm.bmp");

	// Draw D-Star insignia
	displayImage(0U, 30U, "DStar_sm.bmp");

	displayText(0U, 8U, "Listening");
}

void CTFTSerial::setDMR()
{
	// Clear the screen
	clearScreen();

	// Draw MMDVM logo
	displayImage(0U, 0U, "MMDVM_sm.bmp");

	// Draw DMR insignia
	displayImage(0U, 30U, "DMR_sm.bmp");

	displayText(0U, 8U, "1: Listening");
	displayText(0U, 9U, "2: Listening");
}

void CTFTSerial::writeDMR(unsigned int slotNo, unsigned int srcId, bool group, unsigned int dstId, const char* type)
{
	assert(type != NULL);

	char text[20U];
	::sprintf(text, "%u: %s %u -> %s%u", slotNo, type, srcId, group ? "TG" : "", dstId);

	if (slotNo == 1U)
		displayText(0U, 8U, text);
	else
		displayText(0U, 9U, text);
}

void CTFTSerial::clearDMR(unsigned int slotNo)
{
	if (slotNo == 1U)
		displayText(0U, 8U, "1: Listening");
	else
		displayText(0U, 9U, "2: Listening");
}

void CTFTSerial::setFusion()
{
	// Clear the screen
	clearScreen();

	// Draw MMDVM logo
	displayImage(0U, 0U, "MMDVM_sm.bmp");

	// Draw the System Fusion insignia
	displayImage(0U, 30U, "YSF_sm.bmp");

	displayText(0U, 8U, "Listening");
}

void CTFTSerial::writeFusion(const std::string& callsign)
{
	char text[20U];
	::sprintf(text, "%s", callsign.c_str());

	displayText(0U, 8U, text);
}

void CTFTSerial::clearFusion()
{
	// Draw MMDVM logo
	displayImage(0U, 0U, "MMDVM_sm.bmp");

	// Draw the System Fusion insignia
	displayImage(0U, 30U, "YSF_sm.bmp");

	displayText(0U, 8U, "Listening");
}

void CTFTSerial::close()
{
	m_serial.close();
}

void CTFTSerial::clearScreen()
{
	m_serial.write((unsigned char*)"\x1B\x00\xFF", 3U);
}

void CTFTSerial::setForeground(unsigned char colour)
{
	assert(colour >= 0U && colour <= 7U);

	m_serial.write((unsigned char*)"\x1B\x01", 2U);
	m_serial.write(&colour, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::setBackground(unsigned char colour)
{
	assert(colour >= 0U && colour <= 7U);

	m_serial.write((unsigned char*)"\x1B\x02", 2U);
	m_serial.write(&colour, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::setRotation(unsigned char rotation)
{
	assert(rotation >= 0U && rotation <= 3U);

	m_serial.write((unsigned char*)"\x1B\x03", 2U);
	m_serial.write(&rotation, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::setFontSize(unsigned char size)
{
	assert(size >= 0U && size <= 2U);

	m_serial.write((unsigned char*)"\x1B\x04", 2U);
	m_serial.write(&size, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::displayText(unsigned char x, unsigned char y, const char* text)
{
	assert(text != NULL);

	m_serial.write((unsigned char*)"\x1B\x06", 2U);
	m_serial.write(&x, 1U);
	m_serial.write(&y, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
	m_serial.write((unsigned char*)text, ::strlen(text));
}

void CTFTSerial::drawLine(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
	m_serial.write((unsigned char*)"\x1B\x08", 2U);
	m_serial.write(&x1, 1U);
	m_serial.write(&y1, 1U);
	m_serial.write(&x2, 1U);
	m_serial.write(&y2, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::drawBox(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, bool filled)
{
	if (filled)
		m_serial.write((unsigned char*)"\x1B\x0A", 2U);
	else
		m_serial.write((unsigned char*)"\x1B\x09", 2U);

	m_serial.write(&x1, 1U);
	m_serial.write(&y1, 1U);
	m_serial.write(&x2, 1U);
	m_serial.write(&y2, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::drawCircle(unsigned char x, unsigned char y, unsigned char radius, bool filled)
{
	if (filled)
		m_serial.write((unsigned char*)"\x1B\x0C", 2U);
	else
		m_serial.write((unsigned char*)"\x1B\x0B", 2U);

	m_serial.write(&x, 1U);
	m_serial.write(&y, 1U);
	m_serial.write(&radius, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::displayImage(unsigned char x, unsigned char y, const char* filename)
{
	assert(filename != NULL);

	m_serial.write((unsigned char*)"\x1B\x0D", 2U);
	m_serial.write(&x, 1U);
	m_serial.write(&y, 1U);
	m_serial.write((unsigned char*)filename, ::strlen(filename));
	m_serial.write((unsigned char*)"\xFF", 1U);
}

void CTFTSerial::setBrightness(unsigned char brightness)
{
	assert(brightness >= 0U && brightness <= 100U);

	m_serial.write((unsigned char*)"\x1B\x0E", 2U);
	m_serial.write(&brightness, 1U);
	m_serial.write((unsigned char*)"\xFF", 1U);
}
