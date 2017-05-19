var act = require("./action");
var spawn = require("child_process").spawn;
var pages = require("./pages");


var Plot = function(path, args)
{
	/* Members */
	this.path = "";
	this.points = [];
	
	this.label = "";

	this.xlabel = "";
	this.ylabel = "";
	this.zlabel = "";

	this.is3d = 0;

	/* Methods */

	Plot.prototype.action = function( action )
	{
		var x = action.args["x"];
		var y = action.args["y"];
		var z = action.args["z"];
	
		if( this.is3d )
		{
			if( !x || !y || !z)
			{
				throw new Error("A 3D PLOT point must specify x,y,z");
				return;
			}
		
		
			this.points.push({"t":action.time, "x" : x , "y" : y , "z" : z });
		
		}
		else
		{
			if( !x || !y )
			{
				throw new Error("A 3D PLOT point must specify x,y");
				return;
			}
			
			this.points.push({"time":action.time, "x" : x , "y" : y });
		}


	}


	Plot.prototype.render = function( args )
	{
		var ret = "";

		this.points.sort(function(a,b)
						{
							return parseFloat(a.time)-parseFloat(b.time);
						});

		var gnuplot = spawn("gnuplot", ["-p"]);

		gnuplot.stdin.setEncoding("utf8");
		gnuplot.stdout.pipe(process.stdout);
		gnuplot.stderr.pipe(process.stderr);

		var newres = pages.PageManager.getrespath() + ".png"

		if( this.is3d == 0 )
		{
		

			gnuplot.stdin.write("set xlabel \"" + this.xlabel + "\";");
			gnuplot.stdin.write("set ylabel \"" + this.ylabel + "\";");
			gnuplot.stdin.write("set terminal png;");
			gnuplot.stdin.write("set output \"" + newres + "\";");
			gnuplot.stdin.write("set title \"" + this.label + "\";");
			gnuplot.stdin.write("plot '-' using 1:2 w lp lw 3 title \"\";\n");

			for( var i = 0 ; i < this.points.length; i++ )
			{
				var p = this.points[i];
				gnuplot.stdin.write(p.x + " " + p.y + "\n");
			}

			gnuplot.stdin.end();


		}

		this.points = [];

		return "<img src=\"" + newres + "\" alt=\"" + this.label + "\">";
	}


	/* Constructor */

	if( !path )
	{
		throw new Error("No path provided to PLOT");
	}

	this.path = path[0];


	var l = args["label"];

	if(!l)
	{
		throw new Error("PLOTs mush have a label");
		return;
	}
	else
	{
		this.label = l;
	}

	var xl = args["x"];
	var yl = args["y"];
	var zl = args["z"];
 

	if( xl && yl && !zl )
	{
		this.is3d = 0;
		this.xlabel = xl;
		this.ylabel = yl;
	}
	else if( xl && yl && zl )
	{
		this.is3d = 1;
		this.xlabel = xl;
		this.ylabel = yl;
		this.zlabel = zl;
	}
	else
	{
		throw new Error("PLOT do take either x,y or x,y,z Labels");
		return;
	}
}



module.exports.Plot = Plot;



