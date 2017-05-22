
var log = require("./log");
var plot = require("./plot");
var table = require("./table");
var graph = require("./graph");
var img = require("./img");
var act = require("./action");

var Factory = function ()
{
	/* Members */
	this.objects = {};
	this.kinds = {};

	/* Methods */

	Factory.prototype.register = function( name, func )
	{
		this.kinds[name] = func;
	}

	Factory.prototype.dump = function()
	{
		for( var k in this.objects )
		{
			var o = this.objects[k];

			if( o.islog )
			{
				o.dump();
			}
		}
	
	}

	Factory.prototype.parseLine = function(line)
	{
		try
		{
			var action = new act.Action(line);	
		}
		catch(e)
		{
			/* Not a structured output */
			console.log(line);
			return;
		}
		/* Is it a creation ? */
		var hasnew = action.args["new"];

		/* Here we only redirect root object creations */
		if( hasnew && (action.path.length == 1) )
		{
			delete action.args["new"];

			if( this.kinds[hasnew] == undefined )
			{
				throw new Error("No such object kind " + hasnew );
				return;
			}

			var tn = action.path[0];

			if( this.objects[tn] )
			{
				throw new Error("Object " + tn + " already exists");
				return;
			}

			/* If found then create */
			this.objects[tn] = new (this.kinds[hasnew])(action.path,  action.args);

		}
		else
		{
			var hasemit = action.args["emit"];
			var haslog = action.args["log"];
			
			var target = action.path[0];
			var etar = this.objects[target];

			if( etar == undefined )
			{
				console.log("==> " + line );
				throw new Error("No such object " + target );
				return;
			}


			if( hasemit )
			{
				/* It is an emit command */
				if( haslog )
				{
					throw new Error("For LOG emit and log are mutually exclusive");
					return;
				}


				/* Check that target is a log */
				if( etar.islog != 1 )
				{
					throw new Error("Emit targets must be LOGs");
					return;
				}

				var source = this.objects[ hasemit ];

				if( source == undefined )
				{
					throw new Error("No such source object " + hasemit );
					return;
				}

				if( source.render == undefined )
				{
					throw new Error("Object " + hasemit + " has no render method" );
					return;
				}

				var render = (source.render)(action.args);

				etar.push( action.path, action.time, render );
			}
			else
			{
				/* It is a command passing */
				(this.objects[target].action)( action ); 
			}
		}
	}


	/* Constructor */

	this.register("LOG", log.Log);
	this.register("PLOT", plot.Plot);
	this.register("TABLE", table.Table);
	this.register("GRAPH", graph.Digraph);
	this.register("IMG", img.Image);

}


module.exports.Factory = Factory;

