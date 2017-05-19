var act = require("./action");
var spawn = require("child_process").spawn;
var pages = require("./pages");


function genargs( obj )
{
	var ret = "";

	for( k in obj )
	{
		ret += " " + k + "=\"" + obj[k] + "\" "
	}

	return ret;
}


var Digraph = function(path, args)
{
	/* Members */
	this.path = "";
	this.type = "dot";
	this.edges = [];
	this.nodes = {};
	this.commit = 0;
	
	this.label = "";

	/* Methods */

	Digraph.prototype.action = function( action )
	{
		var has_commit = action.args["commit"];	
		var has_new = action.args["new"];	
		var has_edge = action.args["edge"];	

		if( has_commit )
		{
			if( has_edge || has_new )
			{
				throw new Error("DIGRAPH commit command must be used alone" );
				return;	
			}

			this.commit = 1;
			return;
		}
		
		if( has_new )
		{
			if( has_commit || has_edge )
			{
				throw new Error("DIGRAPH new command must be used alone" );
				return;	
			}

			if( has_new != "NODE")
			{
				throw new Error("DIGRAPH objects can only create new nodes not " + has_new );
				return;
			}
	
			if( action.path.length  != 2 )
			{
				throw new Error("DIGRAPH new NODE object mush have a different path" );
				return;
			}


			var nlabel = action.args["label"];

			if( !nlabel )
			{
				return;	
			}
	

			if( this.nodes[nlabel] )
			{
				throw new Error("DIGRAPH NODE " + nlabel + " already declared ");
				return;	
			}

			delete action.args["new"];

			action.args["key"] = action.path[1];
	
			this.nodes[ action.path[1] ] = action.args;
		}


		if( has_edge )
		{
			if( has_new || has_commit )
			{
				throw new Error("DIGRAPH edge command must be used alone" );
				return;	
			}
	
			var e = has_edge.split("-");

			if( e.length != 2 )
			{
				throw new Error("DIGRAPH bad edge sytax (a-b) "  + has_edge);
				return;
			}

			var n1 = this.nodes[ e[0] ];
		
			if( !n1 )
			{
				throw new Error("DIGRAPH NODE was not declared "  + e[0] );
				return;
			}

			var n2 = this.nodes[ e[1] ];
		
			if( !n2 )
			{
				throw new Error("DIGRAPH NODE was not declared "  + e[1] );
				return;
			}

			delete action.args["edge"];
			action.args.src = e[0];
			action.args.dest = e[1];
			this.edges.push( action.args );

		}

	}


	Digraph.prototype.render = function( args )
	{
		var ret = "";


		var newres = pages.PageManager.getrespath() + ".png"
	
		var gviz = spawn(this.type, ["-Tpng", "-o", newres]);

		gviz.stdin.setEncoding("utf8");
		gviz.stdout.pipe(process.stdout);
		gviz.stderr.pipe(process.stderr);


		gviz.stdin.write("Digraph G{\n");

		for( var nl in this.nodes )
		{
			var n = this.nodes[nl];
			var k = n["key"];
			delete n.key;
			var args = genargs(n);
			gviz.stdin.write( k + " [" + args + "]\n");
		}


		for( var i = 0 ; i < this.edges.length; i++ )
		{
			var e = this.edges[i];
			var src = e.src;
			var dest = e.dest;

			delete e.dest;
			delete e.src;
			
			var args = genargs( e );

			gviz.stdin.write( src + "->" + dest + "  ["  + args +  "]\n");
		}

		gviz.stdin.write("}\n");
		gviz.stdin.end();


		this.edges = [];

		return "<img src=\"" + newres + "\" alt=\"" + this.label + "\">";
	}


	/* Constructor */

	if( !path )
	{
		throw new Error("No path provided to GRAPH");
	}

	this.path = path[0];

	var l = args["label"];

	if(!l)
	{
		throw new Error("GRAPHSs mush have a label");
		return;
	}
	else
	{
		this.label = l;
	}

	var type = args["type"];

	if( type )
	{
		this.type = type;
	}
}



module.exports.Digraph = Digraph;



