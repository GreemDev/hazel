#include "hzpch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Hazel/Core/UUID.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/Input.h"
#include "Hazel/Core/Log.h"

#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

#include "Hazel/Physics/Physics2D.h"


#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"

namespace Hazel {

	namespace Utils {

		std::string MonoStringToString(MonoString* string)
		{
			char* cStr = mono_string_to_utf8(string);
			std::string str(cStr);
			mono_free(cStr);
			return str;
		}

	}

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define HZ_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Hazel.InternalCalls::" #Name, Name)

	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		HZ_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;
		
		return entity.GetUUID();
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outRotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		*outRotation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* rotation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Rotation = *rotation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outScale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		*outScale = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* scale)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Scale = *scale;
	}

	static void CircleRendererComponent_GetColor(UUID entityID, glm::vec4* outColor)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		*outColor = entity.GetComponent<CircleRendererComponent>().Color;
	}

	static void CircleRendererComponent_SetColor(UUID entityID, glm::vec4* color) 
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		entity.GetComponent<CircleRendererComponent>().Color = *color;
	}

	static float CircleRendererComponent_GetThickness(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		return entity.GetComponent<CircleRendererComponent>().Thickness;
	}

	static void CircleRendererComponent_SetThickness(UUID entityID, float thickness)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		entity.GetComponent<CircleRendererComponent>().Thickness = thickness;
	}

	static float CircleRendererComponent_GetFade(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		return entity.GetComponent<CircleRendererComponent>().Fade;
	}

	static void CircleRendererComponent_SetFade(UUID entityID, float fade)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		entity.GetComponent<CircleRendererComponent>().Fade = fade;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}
	
	static void Rigidbody2DComponent_GetLinearVelocity(UUID entityID, glm::vec2* outLinearVelocity)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		const b2Vec2& linearVelocity = body->GetLinearVelocity();
		*outLinearVelocity = glm::vec2(linearVelocity.x, linearVelocity.y);
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DComponent_GetType(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return Utils::Rigidbody2DTypeFromBox2DBody(body->GetType());
	}
	
	static void Rigidbody2DComponent_SetType(UUID entityID, Rigidbody2DComponent::BodyType bodyType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->SetType(Utils::Rigidbody2DTypeToBox2DBody(bodyType));
	}

	static MonoString* TextComponent_GetText(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		return ScriptEngine::CreateString(tc.TextString.c_str());
	}

	static void TextComponent_SetText(UUID entityID, MonoString* textString)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		tc.TextString = Utils::MonoStringToString(textString);
	}

	static void TextComponent_GetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		*color = tc.Color;
	}

	static void TextComponent_SetColor(UUID entityID, glm::vec4* color)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		tc.Color = *color;
	}

	static float TextComponent_GetKerning(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		return tc.Kerning;
	}

	static void TextComponent_SetKerning(UUID entityID, float kerning)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		tc.Kerning = kerning;
	}

	static float TextComponent_GetLineSpacing(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		return tc.LineSpacing;
	}

	static void TextComponent_SetLineSpacing(UUID entityID, float lineSpacing)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		HZ_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		HZ_CORE_ASSERT(entity);
		HZ_CORE_ASSERT(entity.HasComponent<TextComponent>());

		auto& tc = entity.GetComponent<TextComponent>();
		tc.LineSpacing = lineSpacing;
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	static void Logger_Trace(MonoString* string) 
	{
		HZ_TRACE(Utils::MonoStringToString(string));
	}

	static void Logger_Info(MonoString* string)
	{
		HZ_INFO(Utils::MonoStringToString(string));
	}

	static void Logger_Warn(MonoString* string)
	{
		HZ_WARN(Utils::MonoStringToString(string));
	}

	static void Logger_Error(MonoString* string)
	{
		HZ_ERROR(Utils::MonoStringToString(string));
	}

	static void Logger_Critical(MonoString* string)
	{
		HZ_CRITICAL(Utils::MonoStringToString(string));
	}

	static void Logger_cout(MonoString* string) 
	{
		std::string logMessage = Utils::MonoStringToString(string);
		std::cout << logMessage << std::endl;
	}


	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypename = fmt::format("Hazel.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				HZ_CORE_ERROR("Could not find component type {}", managedTypename);
				return;
			}
			s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
		}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		HZ_ADD_INTERNAL_CALL(GetScriptInstance);

		HZ_ADD_INTERNAL_CALL(Entity_HasComponent);
		HZ_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		HZ_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		HZ_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		HZ_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		HZ_ADD_INTERNAL_CALL(CircleRendererComponent_GetColor);
		HZ_ADD_INTERNAL_CALL(CircleRendererComponent_SetColor);
		HZ_ADD_INTERNAL_CALL(CircleRendererComponent_GetThickness);
		HZ_ADD_INTERNAL_CALL(CircleRendererComponent_SetThickness);
		HZ_ADD_INTERNAL_CALL(CircleRendererComponent_GetFade);
		HZ_ADD_INTERNAL_CALL(CircleRendererComponent_SetFade);
		
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetLinearVelocity);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_GetType);
		HZ_ADD_INTERNAL_CALL(Rigidbody2DComponent_SetType);
		
		HZ_ADD_INTERNAL_CALL(TextComponent_GetText);
		HZ_ADD_INTERNAL_CALL(TextComponent_SetText);
		HZ_ADD_INTERNAL_CALL(TextComponent_GetColor);
		HZ_ADD_INTERNAL_CALL(TextComponent_SetColor);
		HZ_ADD_INTERNAL_CALL(TextComponent_GetKerning);
		HZ_ADD_INTERNAL_CALL(TextComponent_SetKerning);
		HZ_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
		HZ_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);

		HZ_ADD_INTERNAL_CALL(Input_IsKeyDown);

		HZ_ADD_INTERNAL_CALL(Logger_Trace);
		HZ_ADD_INTERNAL_CALL(Logger_Info);
		HZ_ADD_INTERNAL_CALL(Logger_Warn);
		HZ_ADD_INTERNAL_CALL(Logger_Error);
		HZ_ADD_INTERNAL_CALL(Logger_Critical);
		HZ_ADD_INTERNAL_CALL(Logger_cout);
	}
}
