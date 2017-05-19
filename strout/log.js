
var act = require("./action");
var pages = require("./pages");
var marked = require('marked');

marked.setOptions({
  renderer: new marked.Renderer(),
  gfm: true,
  tables: true,
  breaks: false,
  pedantic: false,
  sanitize: false,
  smartLists: true,
  smartypants: false
});

var Line = function(ts, content)
{
	this.ts = ts;
	this.content = content;
}


var Stream = function( id,  label, md )
{
	/* Members */
	this.lines = [];
	this.id = "";
	this.label = "";
	this.page = null;

	/* Methods */
	Stream.prototype.pushline = function( ts , line )
	{
		this.lines.push( new Line(ts, line) );
	}

	Stream.prototype.dump = function()
	{
		var d = "";

		for( var i = 0 ; i < this.lines.length ; i++ )
		{
			d += this.lines[i].content + "\n";
		}

		if( this.hasmd )
		{
			d = marked(d);
		}

		this.lines = [];
		this.page.append( d );
		this.page.write();
	}


	/* Constructor */
	if(!label)
	{
		throw new Error("Cannot create LOG MEMBER with no label");
		return;
	}

	this.label = label;
	this.id = id;
	this.hasmd = md;
	this.page = pages.PageManager.create( id, label );
};


var Log = function(path, args)
{
	/* Members */
	this.islog = 1;
	this.md = 0;
	this.path = "";
	this.streams = {};
	this.lines = [];
	this.verbosity = 0;
	this.label = "";
	this.commited = 0;

	/* Methods */

	Log.prototype.push = function( path, ts,  data )
	{
		var lpath = path.join(":");
		var ts = this.streams[ lpath ];

		if( !ts )
		{
			throw new Error("No such target stream " + ts );
			return;
		}
	
		ts.pushline( ts,  data );	
	}

	Log.prototype.action = function( action )
	{
		var hascommit = action.args["commit"];
		var hasdump = action.args["dump"];
		var hasmd = action.args["markdown"];
		var hasnew = action.args["new"];
		var lpath = action.path.join(":");

		if( hasdump )
		{
			this.dump();
			return;
		}

		if( hasmd != undefined )
		{
			this.md = 1;
		}

		if( hascommit != undefined )
		{
			this.commited = 1;
			return;
		}
		else if( hasnew == "MEMBER" )
		{
			if( action.path.length == 1 )
			{
				throw new Error("When creating a LOG MEMBER you must give a subpath");
			}

			if( lpath == this.path )
			{
				throw new Error("non sufixed STREAM is reserved for the default STREAM");
				return;
			}

			this.streams[ lpath ] = new Stream( lpath, action.args["label"], this.hasmd );	
		
			/* Done */
			return;
		}
		else if( hasnew != undefined )
		{
			throw new Error("No such constructor for LOG : " + hasnew );
		}

		if( !this.commited )
		{
			throw new Error("You must commit a LOG to generate outputs");
			return;
		}



		var l = action.args["log"];

		if( !l )
		{
			throw new Error("A log entry must define a log parameter");
			return;
		}

		this.push( action.path, action.time, l );

	}

	Log.prototype.dump = function()
	{
		for( s in this.streams )
		{
			var ts = this.streams[s];
			ts.dump();
		}
	}

	/* Constructor */

	if( !path )
	{
		throw new Error("No path provided to LOG");
	}

	this.path = path[0];


	var l = args["label"];

	if(!l)
	{
		throw new Error("LOGs mush have a label");
		return;
	}
	else
	{
		this.label = l;
	}

	if( args["verbosity"] )
	{
		this.verbosity = args["verbosity"];
	}

	if( args["markdown"] )
	{
		this.hasmd = 1;
	}
	console.log( args );

	this.streams[path[0]] = new Stream(path[0], l, this.hasmd);

}



module.exports.Log = Log;



