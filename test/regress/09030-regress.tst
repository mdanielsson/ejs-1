/*
 *  Was failing to parse the ".<"
 */

var xml: XML = <para><def>Woman</def> Similar to a man, but of a different sex.</para>
assert(xml.def == "Woman")
