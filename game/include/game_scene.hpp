#pragma once

#include "engine_context.hpp"
#include "object_2d.hpp"

class Scene : public SceneNodeInterface<Object2D> {
   public:
    virtual ~Scene() noexcept override = default;
    Scene(ContextPtr context);

   private:
    ContextPtr m_context;
};
