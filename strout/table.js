var act = require("./action");

var Table = function(path, args)
{
	/* Members */
	this.path = "";
	this.rows = [];
	this.cols = [];
	this.label = "";

	/* Methods */

	Table.prototype.action = function( action )
	{
		var ent = {};

		ent.time = action.time;

		for( var k=0; k < this.cols.length ; k++ )
		{
			var id = this.cols[k].id; 
			var v = action.args[ id ];

			if( v == undefined )
			{
				throw new Error("Missing value " + id + " in TABLE row");
				return;
			}

			ent[id] = v;
		}

		this.rows.push( ent );
	}


	Table.prototype.render = function(arg)
	{

		this.rows.sort(function(a,b){return parseFloat(a.time) - parseFloat(b.time);});

		var ret = "<table>\n"
	

		ret += "<tr>\n";
	
		for( var i = 0 ; i < this.cols.length ; i++ )
		{
			ret+= "<th>" + this.cols[i].label + "</th>\n"
		
		}

		ret += "</tr>\n";

		for( var i = 0 ; i < this.rows.length ; i++ )
		{
			ret += "   <tr>\n"
			for( var j = 0 ; j < this.cols.length ; j++ )
			{
				var v = this.rows[i][ this.cols[j].id  ];
				ret+= "      <td>" + v  + "</td>\n"
			}
			ret += "   </tr>\n"
		}
	

		ret += "</table>\n"

		/* Clear rows */
		this.rows = [];

		return ret;
	
	}

	/* Constructor */

	if( !path )
	{
		throw new Error("No path provided to TABLE");
	}

	this.path = path[0];


	var l = args["label"];

	if(!l)
	{
		throw new Error("TABLESs mush have a label");
		return;
	}
	else
	{
		this.label = l;
	}

	/* Not to add as table coll */

	delete args["label"];

	for( var k in args )
	{
		var d = args[k];

		var o = {};
		o["id"] = k;
		o["label"] = d;

		this.cols.push(o);
	}

}



module.exports.Table = Table;



