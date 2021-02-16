#include "preferences.h" // class's header file

// class constructor
Preferences::Preferences()
{
	this->flagUseGuards=true;
	this->flagExitNodes=true;
	this->flagAllowNonValidEntry=false;
	this->flagAllowNonValidMiddle=true;
	this->flagAllowNonValidExit=false;
	this->flagFastCircuit=false;
	this->flagRequireStable=false;
}

Preferences::Preferences(Preferences* prefs)
{
	this->flagUseGuards=prefs->flagUseGuards;
	this->flagExitNodes=prefs->flagExitNodes;
	this->flagAllowNonValidEntry=prefs->flagAllowNonValidEntry;
	this->flagAllowNonValidMiddle=prefs->flagAllowNonValidMiddle;
	this->flagAllowNonValidExit=prefs->flagAllowNonValidExit;
	this->flagFastCircuit=prefs->flagFastCircuit;
	this->flagRequireStable=prefs->flagRequireStable;
}


// class destructor
Preferences::~Preferences()
{

}

bool Preferences::useGuards() const
{
	return this->flagUseGuards;
}
bool Preferences::exitNodes() const
{
	return this->flagExitNodes;
}

bool Preferences::allowNonValidEntry() const
{
	return this->flagAllowNonValidEntry;
}
bool Preferences::allowNonValidMiddle() const
{
	return this->flagAllowNonValidMiddle;
}
bool Preferences::allowNonValidExit() const
{
	return this->flagAllowNonValidExit;
}
bool Preferences::fastCircuit() const
{
	return this->flagFastCircuit;
}
bool Preferences::requireStable() const
{
	return this->flagRequireStable;
}

