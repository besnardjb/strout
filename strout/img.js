var act = require("./action");
var canvas = require("canvas");
var fs = require("fs");
var pages = require("./pages");

var Image = function(path, args)
{
	/* Members */
	this.path = "";
	this.rows = [];
	this.label = "";
	this.sx = 0;
	this.sy = 0;

	this.can = null;
	this.ctx = null;

	/* Methods */

	Image.prototype.action = function( action )
	{
		var ent = {};

		ent.time = action.time;

		var gs = action.args["gs"];
		
		var r = action.args["r"];
		var g = action.args["g"];
		var b = action.args["b"];
		
		if( gs )
		{
			if( r | g | b )
			{
				throw new Error("IMG cannot provide both gs and either r,g or b");
				return;
			}

			gs = parseInt(gs)
		
			r = gs;
			g = gs;
			b = gs;
		}
		else
		{
			if( !r | !g | !b )
			{
				throw new Error("IMG actions mush have all r,g,b atributes or gr for grayscale");
				return;
			}

			r = parseInt(r);
			g = parseInt(g);
			b = parseInt(b);
		}

		var x = action.args["x"];
		var y = action.args["y"];

		if( !x | !y )
		{
			throw new Error("IMG mush have x,y coordinates");
			return;
		}

		x = parseInt(x);
		y = parseInt(y);

		if( (this.sx <= x) || (x < 0) )
		{
			throw new Error("IMG x value it outside range : " + x + " should be in (0, " + this.sx + ")");
			return;
		}

		if( (this.sy <= y) || (y < 0) )
		{
			throw new Error("IMG y value it outside range : " + y + " should be in (0, " + this.sy + ")");
			return;
		}


		this.ctx.fillStyle = "#" + r.toString(16) + g.toString(16) + b.toString(16);
		this.ctx.fillRect(x, y, 1, 1)
	}


	Image.prototype.render = function(arg)
	{
		console.log("RENDER");
		
		var newres = pages.PageManager.getrespath() + ".jpg"

		this.can.createJPEGStream().pipe(fs.createWriteStream(newres))
		return "<img src=\"" + newres + "\" alt=\"" + this.label + "\">";
	}

	/* Constructor */

	if( !path )
	{
		throw new Error("No path provided to IMG");
	}

	this.path = path[0];


	var l = args["label"];

	if(!l)
	{
		throw new Error("IMGs mush have a label");
		return;
	}
	else
	{
		this.label = l;
	}

	var sx = args["sx"];

	if(!sx)
	{
		throw new Error("IMGs mush have an X dimension (sx)");
		return;
	}
	else
	{
		this.sx = parseInt(sx);
	}

	var sy = args["sy"];

	if(!sy)
	{
		throw new Error("IMGs mush have an Y dimension (sy)");
		return;
	}
	else
	{
		this.sy = parseInt(sy);
	}

	this.can = new canvas(this.sx, this.sy);
	this.ctx = this.can.getContext('2d');
}

module.exports.Image = Image;



