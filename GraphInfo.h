#ifndef __GRAPH_INFO__
#define __GRAPH_INFO__

#include "Container.h"

class LightNode;

class GraphInfo {
public:
	Container<LightNode*> lights;
	bool hasEnvironment;
	GraphInfo(const GraphInfo& that);
	GraphInfo(bool has = false);
	GraphInfo(LightNode* light, bool has = false);
	GraphInfo& mergeWith(const GraphInfo& that);
};

//PUBLIC

inline
GraphInfo::GraphInfo(const GraphInfo& that) : lights(that.lights), hasEnvironment(that.hasEnvironment)
{ }
inline
GraphInfo::GraphInfo(bool has) : hasEnvironment(has) { }
inline
GraphInfo::GraphInfo(LightNode* light, bool has) : lights(light), hasEnvironment(has) { }

inline
GraphInfo& GraphInfo::mergeWith(const GraphInfo& that) {
	lights.push_block(that.lights);
	hasEnvironment = hasEnvironment || that.hasEnvironment;
	return *this;
}

#endif