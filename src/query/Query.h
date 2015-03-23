/*
 * Query.h
 *
 *  Created on: Mar 22, 2015
 *      Author: jonno
 */

#ifndef QUERY_QUERY_H_
#define QUERY_QUERY_H_

#include "../Node.h"
#include <memory.h>

namespace Query {

class Query {
private:
	std::unique_ptr<Node> myNode;

	static std::string formatNodeOutput(Node* n);
public:
	Query(std::string ip, std::string port);
	virtual ~Query();

	void Start();
};

} /* namespace Query */

#endif /* QUERY_QUERY_H_ */
