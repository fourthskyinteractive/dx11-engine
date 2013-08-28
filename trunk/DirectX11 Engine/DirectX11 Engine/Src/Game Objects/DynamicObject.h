#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include "../Game/Definitions.h"
#include <vector>
using namespace std;
#include <atlbase.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <dxgi.h>
using namespace DirectX;

class DynamicObject
{
public:
	DynamicObject();
	DynamicObject(const DynamicObject& _dynamicObject);
	~DynamicObject();

private:
	char componentsBitFlag;

public:
	void Render();
	void Update(float _dt);

};

#endif
