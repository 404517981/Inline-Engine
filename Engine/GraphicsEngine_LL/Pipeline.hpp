#pragma once

#include "Task.hpp"
#include <string>
#include <vector>
#include <iterator>
#include <lemon/list_graph.h>
#include <Graph/Node.hpp>


namespace inl {
namespace gxeng {


class GraphicsNodeFactory;


class Pipeline {
public:
	class NodeIterator : public std::bidirectional_iterator_tag {
	private:
		friend class inl::gxeng::Pipeline;
		NodeIterator(Pipeline* parent, lemon::ListDigraph::NodeIt graphIt);
	public:
		NodeIterator();
		NodeIterator(const NodeIterator&) = default;
		NodeIterator& operator=(const NodeIterator&) = default;

		const exc::NodeBase& operator*();
		const exc::NodeBase* operator->();

		bool operator==(const NodeIterator&);
		bool operator!=(const NodeIterator&);

		NodeIterator& operator++();
		NodeIterator operator++(int);
	private:
		lemon::ListDigraph::NodeIt m_graphIt;
		Pipeline* m_parent;
	};

public:
	Pipeline() = default;
	~Pipeline();

	void CreateFromDescription(const std::string& jsonDescription);

	void SetFactory(GraphicsNodeFactory* factory);
	GraphicsNodeFactory* GetFactory() const;


	NodeIterator Begin();
	NodeIterator End();

	NodeIterator AddNode(const std::string& fullName);
	void Erase(NodeIterator node);
	void AddLink(NodeIterator node1, int port1, NodeIterator node2, int port2);		
private:
	void ExpandTaskGraph();
	void CalculateDependencies();


	lemon::ListDigraph m_dependencyGraph;
	lemon::ListDigraph::NodeMap<exc::NodeBase*> m_dependencyGraphNodes;
	lemon::ListDigraph m_taskGraph;
	lemon::ListDigraph::NodeMap<ElementaryTask> m_taskGraphMaps;
	
	GraphicsNodeFactory* m_factory;
};



} // namespace gxeng
} // namespace inl
