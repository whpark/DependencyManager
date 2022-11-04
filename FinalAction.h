#pragma once

//====================================================================================
class xFinalAction {
protected:
	std::function<void(void)> func;
public:
	xFinalAction(std::function<void(void)>&& func) : func(std::move(func)) {}
	~xFinalAction() { Release(); }
	bool Release() {
		if (!func)
			return false;
		func();
		func = {};
		return true;
	}
};
