//
// Created by steph on 5/22/2025.
//

#include "ActorEditor.h"

#include <imgui.h>

#include "core/objects/World.h"
#include "../../core/objects/prefabs/Prefab.h"
#include "core/objects/actor/Actor.h"
#include "core/objects/component/Component.h"
#include "core/services/InputService.h"
#include "core/services/ObjectService.h"
#include "core/services/ServiceLocator.h"
#include "editor/ClassList.h"
#include "editor/Editor.h"
#include "editor/EditorViewport.h"
#include "editor/WorldOutline.h"
#include "editor/detail_panel/DetailPanel.h"
#include "rendering/Renderer.h"
#include "rendering/SceneRenderer.h"

namespace Vox
{
    ActorEditor::ActorEditor(Prefab* actorClass)
    {
        currentPrefab = actorClass;
        world = std::make_shared<World>();
        rootObject = world->CreateObject(actorClass);
        rootObject->native = true;

        componentList = std::make_unique<ClassList>();
        componentList->title = "Components";
        componentList->objectClassPayloadType = "COMPONENT_CLASS_NAME";
        componentList->SetClassFilter([](const std::shared_ptr<ObjectClass>& objectClass)
        {
           return objectClass->IsA<Component>();
        });

        viewport = std::make_shared<EditorViewport>();
        viewport->OnObjectSelected = [this](const std::shared_ptr<Object>& object)
            {
                outline->SetSelectedObject(object);
            };
        viewport->SetDragFilter("COMPONENT_CLASS_NAME");
        viewport->SetOnDroppedDelegate([this](const void* classData)
        {
            const auto objectClassName = static_cast<const char*>(classData);
            std::shared_ptr<ObjectClass> componentClass = ServiceLocator::GetObjectService()->GetObjectClass(objectClassName);
            if (!componentClass)
            {
                return;
            }

            if (auto rootActor = std::dynamic_pointer_cast<Actor>(rootObject))
            {
                if (componentClass->IsA<SceneComponent>())
                {
                    const std::shared_ptr<SceneComponent> newComponent = rootActor->AttachComponent(componentClass.get());
                    newComponent->native = false;
                }
                else
                {
                    ObjectInitializer objectInitializer;
                    objectInitializer.parent = rootActor.get();
                    objectInitializer.world = rootActor->GetWorld();
                    const std::shared_ptr<Object> newComponent = componentClass->GetConstructor()(objectInitializer);
                    newComponent->native = false;
                    rootActor->AddChild(newComponent);
                }
            }
        });

        outline = std::make_unique<WorldOutline>();
        outline->InitializeGizmos(world.get());

        world->GetRenderer()->viewport = viewport;
        const std::shared_ptr<Camera>& defaultCamera = world->GetRenderer()->GetCurrentCamera();
        constexpr glm::vec3 cameraDefaultRotation = {glm::radians(22.5f), glm::radians(-45.0f), 0.0f};
        defaultCamera->SetRotation(cameraDefaultRotation);
        defaultCamera->SetPosition(defaultCamera->GetForwardVector() * 5.0f);

        ServiceLocator::GetRenderer()->RegisterScene(world->GetRenderer());

        InputService* inputService = ServiceLocator::GetInputService();
        saveDelegate = inputService->RegisterKeyboardCallback(SDL_SCANCODE_S, [this](const bool pressed)
        {
           if (pressed && (ServiceLocator::GetInputService()->IsKeyPressed(SDL_SCANCODE_LCTRL) || ServiceLocator::GetInputService()->IsKeyPressed(SDL_SCANCODE_RCTRL)))
           {
               SavePrefab();
           }
        });

        deleteDelegate = inputService->RegisterKeyboardCallback(SDL_SCANCODE_DELETE, [this](const bool pressed)
        {
            if (!pressed)
            {
                return;
            }

            std::weak_ptr<Object> selected = outline->GetSelectedObject();
            if (selected.expired())
            {
                return;
            }

            std::shared_ptr<Object> selectedObjectLock = selected.lock();
            if (selectedObjectLock->native)
            {
                return;
            }

            if (selectedObjectLock->GetParent()->RemoveChild(selectedObjectLock.get()))
            {
                VoxLog(Display, Game, "object deleted");
            }
        });
    }

    ActorEditor::~ActorEditor()
    {
        InputService* inputService = ServiceLocator::GetInputService();
        inputService->UnregisterKeyboardCallback(SDL_SCANCODE_S, saveDelegate);
        inputService->UnregisterKeyboardCallback(SDL_SCANCODE_DELETE, deleteDelegate);
    }

    void ActorEditor::Draw()
    {

        ImGui::BeginChild("ActorEditorClassFrame", ImVec2(100, 0), ImGuiChildFlags_ResizeX);
        componentList->Draw();
        ImGui::EndChild();
        ImGui::SameLine();

        ImGui::BeginChild("ActorEditorViewportFrame", ImVec2(200, 0), ImGuiChildFlags_ResizeX);
        viewport->Draw(world);
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("ActorEditorRightPane", ImVec2(0, 0));
            ImGui::BeginChild("ActorEditorOutlineFrame", ImVec2(0, 100), ImGuiChildFlags_ResizeY);
            outline->Draw(world.get());
            ImGui::EndChild();

            ImGui::BeginChild("ActorEditorDetailFrame"), ImVec2(0, 0);
            const std::string tabName = fmt::format("{}###ActorDetailPanelTab", outline->GetSelectedObject().expired() ?
                "No Object Selected" :
                outline->GetSelectedObject().lock()->GetDisplayName());
            Editor::BeginEmptyTab(tabName);
            if (!outline->GetSelectedObject().expired())
            {
                DetailPanel::Draw(outline->GetSelectedObject().lock().get());
            }
            Editor::EndEmptyTab();
        ImGui::EndChild();

        ImGui::EndChild();
    }

    void ActorEditor::SavePrefab()
    {
        currentPrefab->SaveChanges(rootObject.get());
        currentPrefab->SaveToFile(currentPrefab->GetName());
    }
} // Vox