#include <string>

class Page {
	public:
		float TemperatureSaved[288];
		float HumiditySaved[288];
		int timeSaved[288][3];
		int nextSaved;
		float *temperatureNow;
		float *humidityNow;
		int saveCounter;
	
	Page()
	{
		for (int i = 0; i<288; i++)
		{
			TemperatureSaved[i] = 0;
			HumiditySaved[i] = 0;
			for (int j = 0; j<3; j++)
			{
				timeSaved[i][j] = 0;
			}
			
		}
		nextSaved = 0;
		saveCounter = 0;

	}

	void getTime(int *a)
	{
		
		for (int k = 0; k<3; k++)
		{
			timeSaved[nextSaved][k] = a[k];
		}

		if (nextSaved == 0 && HumiditySaved[0] > 0)
		{
			while(timeSaved[0][0] < timeSaved[287][0])
			{
				timeSaved[0][0] += 24;
			}
		}
		else if(nextSaved > 0)
		{
			while(timeSaved[nextSaved][0] < timeSaved[nextSaved-1][0])
			{
				timeSaved[nextSaved][0] += 24;
			}
		}		
	}

	void save(int * a)
	{
		
		if ((saveCounter % 1) == 0)
		{
			saveCounter = 0;
			TemperatureSaved[nextSaved] = *temperatureNow;
			HumiditySaved[nextSaved] = *humidityNow;
			getTime(a);

			if (nextSaved == 287) {nextSaved = 0;}
			else {nextSaved++;}	
		}
		saveCounter++;
		
	}


	String floatToString(float f)
	{
		char a [50];
		sprintf(a, "%f", f);
		String b = a;
		return b;
	}

	String intToString(int d)
	{
		char a [50];
		sprintf(a, "%d", d);
		String b = a;
		return b;
	}

	String generateTimeString(int i)
	{
		String timeString = "[";

		for (int k = 0; k < 3; k++)
		{
			timeString +=intToString(timeSaved[i][k]) + ",";
		}

		timeString += "]";

		return timeString;
	}

	String generateRows()
	{
		String rows;
		if (TemperatureSaved[nextSaved] != 0)
		{
			for (int i = nextSaved; i<288; i++)
			{
				
				rows += "[" + generateTimeString(i) + "," + floatToString(TemperatureSaved[i]) + "," + floatToString(HumiditySaved[i]) + "],";
			}
		}

		for (int i = 0; i < nextSaved ; i++)
		{
			rows += "[" + generateTimeString(i) + "," + floatToString(TemperatureSaved[i]) + "," + floatToString(HumiditySaved[i]) + "],";
		}
		return rows;
	}

	String writeout()
	{
		return R"(
		<HTML>
		<HEAD>
			<TITLE>Room Conditions</TITLE>
			<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
			<script type="text/javascript">

				google.charts.load('current', {packages: ['corechart', 'line']});
				google.charts.setOnLoadCallback(drawCurveTypes);
				
				function drawCurveTypes() {
					var data = new google.visualization.DataTable();
					data.addColumn('timeofday', 'X');
					data.addColumn('number', 'Temperature');
					data.addColumn('number', 'Humidity');

					data.addRows([)"
						+ generateRows() +
					R"(]);

					var options = 
					{
						colors:['#cc0000', '#005ce6'],
						height:500,
						hAxis: {
						title: 'Time'
						},
						vAxis: {
						title: 'temperature/humidity'
						},
						series: {
						1: {curveType: 'function'}
						}
					};

					var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
					chart.draw(data, options);
				}
			</script>
		</HEAD>
		<BODY>
		
			<CENTER>
					<B style="color: #e6693e ">Temperature: )" + floatToString(*temperatureNow) + 
					R"( </B> <B style="color: #005ce6 "> Humidity: )" + floatToString(*humidityNow) + R"(</B>
			</CENTER>
			<div id="chart_div"></div>	
		</BODY>
		</HTML>
		)"; 
	}


};

