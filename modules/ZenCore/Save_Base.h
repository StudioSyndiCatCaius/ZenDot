#ifndef SAVEBASE_H
#define SAVEBASE_H

#include "core/io/resource.h"

class ZSave_Base : public Resource {
	GDCLASS(ZSave_Base, Resource);


protected:
	static void _bind_methods();

public:

	//ZSave_Base();

	Dictionary VARS;
	void set_VARS(const Dictionary &p_VARS) { VARS=p_VARS;}
	Dictionary get_VARS() const { return VARS; }
};


#endif
