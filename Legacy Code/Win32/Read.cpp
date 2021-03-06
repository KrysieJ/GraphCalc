while (!infile.eof()) {
	line = GetLine(&infile);

	var = line.Left(line.Find("="));
	val = line.Right(line.GetLength() - line.Find("=") - 1);

	if (line.IsEmpty() || line[0] == '[' || val.IsEmpty())
		;
	else if (var == "pallete") {
		long double en;
		m_Calculator.StringToDecimal(val, 10, en);
		m_CurrentPalete = (PaleteE)int(en);
	}
	else if (var == "sound") {
		if (val == "1")
			m_SndEffects = true;
		else
			m_SndEffects = false;
	}
	else if (var == "tooltips") {
		if (val == "1")
			m_ToolTips.On(true);
		else
			m_ToolTips.On(false);
	}
	else if (var == "longoutput") {
		if (val == "1")
			m_OutputV.m_LongOutput = true;
		else
			m_OutputV.m_LongOutput = false;
	}
	else if (var == "base") {
		long double vl;
		m_Calculator.StringToDecimal(val, 10, vl);

		if (int(vl) < 2 || int(vl) > 36)
			vl = 10;

		base = int(vl);
	}
	else if (var == "decimalplaces") {
		long double vl;
		m_Calculator.StringToDecimal(val, 10, vl);

		if (int(vl) < 0 || int(vl) > 15)
			vl = 5;

		m_Calculator.m_DecimalPlaces = int(vl);
		m_ModeOptionsDlg.m_DecPlaces = int(vl);
	}
	else if (var == "float") {
		if (val == "1") {
			m_ModeOptionsDlg.m_Float = true;
			m_Calculator.m_Float = true;
		}
		else {
			m_ModeOptionsDlg.m_Float = false;
			m_Calculator.m_Float = false;
		}
	}
	else if (var == "scientific") {
		if (val == "1") {
			m_ModeOptionsDlg.m_OutputMode = 0;
			m_Calculator.m_Normal = true;
		}
		else {
			m_ModeOptionsDlg.m_OutputMode = 1;
			m_Calculator.m_Normal = false;
		}
	}
	else if (var == "trigstate") {
		if (val == "1") {
			m_ModeOptionsDlg.m_TrigUnits = 1;
			m_Calculator.m_trig_state = DEGREE;
		}
		else {
			m_ModeOptionsDlg.m_TrigUnits = 0;
			m_Calculator.m_trig_state = RADIAN;
		}
	}
	else if (var == "y1disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[0].disp = true;
		else
			m_GraphViewList[0].m_Equations[0].disp = false;
	}
	else if (var == "y2disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[1].disp = true;
		else
			m_GraphViewList[0].m_Equations[1].disp = false;
	}
	else if (var == "y3disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[2].disp = true;
		else
			m_GraphViewList[0].m_Equations[2].disp = false;
	}
	else if (var == "y4disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[3].disp = true;
		else
			m_GraphViewList[0].m_Equations[3].disp = false;
	}
	else if (var == "y5disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[4].disp = true;
		else
			m_GraphViewList[0].m_Equations[4].disp = false;
	}
	else if (var == "y6disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[5].disp = true;
		else
			m_GraphViewList[0].m_Equations[5].disp = false;
	}
	else if (var == "y7disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[6].disp = true;
		else
			m_GraphViewList[0].m_Equations[6].disp = false;
	}
	else if (var == "y8disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[7].disp = true;
		else
			m_GraphViewList[0].m_Equations[7].disp = false;
	}
	else if (var == "y9disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[8].disp = true;
		else
			m_GraphViewList[0].m_Equations[8].disp = false;
	}
	else if (var == "y10disp") {
		if (val == "1")
			m_GraphViewList[0].m_Equations[9].disp = true;
		else
			m_GraphViewList[0].m_Equations[9].disp = false;
	}
	else if (var == "y1")
		m_GraphViewList[0].m_Equations[0].eq = val;
	else if (var == "y2")
		m_GraphViewList[0].m_Equations[1].eq = val;
	else if (var == "y3")
		m_GraphViewList[0].m_Equations[2].eq = val;
	else if (var == "y4")
		m_GraphViewList[0].m_Equations[3].eq = val;
	else if (var == "y5")
		m_GraphViewList[0].m_Equations[4].eq = val;
	else if (var == "y6")
		m_GraphViewList[0].m_Equations[5].eq = val;
	else if (var == "y7")
		m_GraphViewList[0].m_Equations[6].eq = val;
	else if (var == "y8")
		m_GraphViewList[0].m_Equations[7].eq = val;
	else if (var == "y9")
		m_GraphViewList[0].m_Equations[8].eq = val;
	else if (var == "y10")
		m_GraphViewList[0].m_Equations[9].eq = val;
	else if (var == "xmin")
		m_Calculator.StringToDecimal(val, 10, m_GraphViewList[0].m_Xmin);
	else if (var == "xmax")
		m_Calculator.StringToDecimal(val, 10, m_GraphViewList[0].m_Xmax);
	else if (var == "xscl")
		m_Calculator.StringToDecimal(val, 10, m_GraphViewList[0].m_Xscl);
	else if (var == "ymin")
		m_Calculator.StringToDecimal(val, 10, m_GraphViewList[0].m_Ymin);
	else if (var == "ymax")
		m_Calculator.StringToDecimal(val, 10, m_GraphViewList[0].m_Ymax);
	else if (var == "yscl")
		m_Calculator.StringToDecimal(val, 10, m_GraphViewList[0].m_Yscl);
	else if (var == "coordinates") {
		if (val == "1")
			m_GraphViewList[0].m_Coords = true;
		else
			m_GraphViewList[0].m_Coords = false;
	}
	else if (var == "grid") {
		if (val == "1")
			m_GraphViewList[0].m_Grid = true;
		else
			m_GraphViewList[0].m_Grid = false;
	}
	else if (var == "axis") {
		if (val == "1")
			m_GraphViewList[0].m_Axis = true;
		else
			m_GraphViewList[0].m_Axis = false;
	}
	else if (var == "labels") {
		if (val == "1")
			m_GraphViewList[0].m_Labels = true;
		else
			m_GraphViewList[0].m_Labels = false;
	}
	else if (var == "cursorpos") {
		if (val == "1")
			m_GraphViewList[0].m_CursorPos = true;
		else
			m_GraphViewList[0].m_CursorPos = false;
	}
	else if (var == "width") {
		long double vl;
		m_Calculator.StringToDecimal(val, 10, vl);

		if (int(vl) < 1 || int(vl) > 50)
			vl = 1;

		m_GraphViewList[0].m_LineThickness = int(vl);
	}
	else if (var == "zoomprec") {
		long double vl;
		m_Calculator.StringToDecimal(val, 10, vl);

		if (vl <= 0.0)
			vl = 0.05;

		m_GraphViewList[0].m_ZoomDecPrec = vl;
	}
	else if (var == "resolution") {
		long double vl;
		m_Calculator.StringToDecimal(val, 10, vl);

		if (int(vl) <= 0 || int(vl) > 100)
			vl = 100;

		m_GraphViewList[0].m_LinePrec = int(vl);
	}
	else if (var == "zoom") {
		long double vl;
		m_Calculator.StringToDecimal(val, 10, vl);

		if (int(vl) <= 0)
			vl = 200;

		m_GraphViewList[0].m_ZoomPerc = int(vl);
	}
	else if (var == "y1color")
		m_GraphViewList[0].m_pen_colors[0] = ParseRGB(val);
	else if (var == "y2color")
		m_GraphViewList[0].m_pen_colors[1] = ParseRGB(val);
	else if (var == "y3color")
		m_GraphViewList[0].m_pen_colors[2] = ParseRGB(val);
	else if (var == "y4color")
		m_GraphViewList[0].m_pen_colors[3] = ParseRGB(val);
	else if (var == "y5color")
		m_GraphViewList[0].m_pen_colors[4] = ParseRGB(val);
	else if (var == "y6color")
		m_GraphViewList[0].m_pen_colors[5] = ParseRGB(val);
	else if (var == "y7color")
		m_GraphViewList[0].m_pen_colors[6] = ParseRGB(val);
	else if (var == "y8color")
		m_GraphViewList[0].m_pen_colors[7] = ParseRGB(val);
	else if (var == "y9color")
		m_GraphViewList[0].m_pen_colors[8] = ParseRGB(val);
	else if (var == "y10color")
		m_GraphViewList[0].m_pen_colors[9] = ParseRGB(val);

	else if (var == "backgroundclr")
		m_GraphViewList[0].m_BackgroundClr = ParseRGB(val);
	else if (var == "axisclr")
		m_GraphViewList[0].m_AxisStyle.color = ParseRGB(val);
	else if (var == "gridclr")
		m_GraphViewList[0].m_GridStyle.color = ParseRGB(val);
	else if (var == "coordsclr")
		m_GraphViewList[0].m_CoordsClr = ParseRGB(val);
	else if (var == "textclr")
		m_GraphViewList[0].m_TextClr = ParseRGB(val);
	else if (var == "areaclr")
		m_GraphViewList[0].m_AreaClr = ParseRGB(val);
}

// check the range values to make sure they are valid
if (m_GraphViewList[0].m_Xmax > 99999999999999999.0)
m_GraphViewList[0].m_Xmax = 99999999999999999.0;
if (m_GraphViewList[0].m_Xmin < -99999999999999999.0)
	m_GraphViewList[0].m_Xmin = -99999999999999999.0;
if (m_GraphViewList[0].m_Xscl <= 0)
m_GraphViewList[0].m_Xscl = 1;
if (m_GraphViewList[0].m_Ymax > 99999999999999999.0)
m_GraphViewList[0].m_Ymax = 999999999999999990.0;
if (m_GraphViewList[0].m_Ymin < -999999999999999990.0)
	m_GraphViewList[0].m_Ymin = -99999999999999999.0;
if (m_GraphViewList[0].m_Yscl <= 0)
m_GraphViewList[0].m_Yscl = 1;

if (m_GraphViewList[0].m_Xmax <= m_GraphViewList[0].m_Xmin) {
	long double max = m_GraphViewList[0].m_Xmin;
	m_GraphViewList[0].m_Xmin = m_GraphViewList[0].m_Xmax;
	m_GraphViewList[0].m_Xmax = max + 1;
}
if (m_GraphViewList[0].m_Ymax <= m_GraphViewList[0].m_Ymin) {
	long double max = m_GraphViewList[0].m_Ymin;
	m_GraphViewList[0].m_Ymin = m_GraphViewList[0].m_Ymax;
	m_GraphViewList[0].m_Ymax = max + 1;
}