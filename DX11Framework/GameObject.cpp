#include "GameObject.h"

std::vector<SP_GAMEOBJECT>& GameObject::GetChildren()
{
	return _children;
}


void GameObject::SetParent(const SP_GAMEOBJECT& parent)
{
	_parent = parent;

	if (parent)
		_parent->AddChild(GetPtr());
}

void GameObject::AddChild(SP_GAMEOBJECT child)
{
	_children.emplace_back(child);
}

SP_GAMEOBJECT GameObject::GetParent()
{
	return _parent;
}