namespace Hazel
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; }
		public ulong EntityId => Entity.ID;
	}

	public class TransformComponent : Component
	{
		public Vector3 Translation
		{
			get
			{
				InternalCalls.TransformComponent_GetTranslation(EntityId, out Vector3 translation);
				return translation;
			}
			set => InternalCalls.TransformComponent_SetTranslation(EntityId, ref value);
		}

		public Vector3 Rotation
		{
			get
			{
				InternalCalls.TransformComponent_GetRotation(EntityId, out Vector3 rotation);
				return rotation;
			}
			set => InternalCalls.TransformComponent_SetRotation(EntityId, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(EntityId, out Vector3 scale);
				return scale;
			}
			set => InternalCalls.TransformComponent_SetScale(EntityId, ref value);
		}
	}

	public class Rigidbody2DComponent : Component
	{
		public enum BodyType { Static = 0, Dynamic, Kinematic }

		public Vector2 LinearVelocity
		{
			get
			{
				InternalCalls.Rigidbody2DComponent_GetLinearVelocity(EntityId, out Vector2 velocity);
				return velocity;
			}
		}
		
		public BodyType Type
		{
			get => InternalCalls.Rigidbody2DComponent_GetType(EntityId);
			set => InternalCalls.Rigidbody2DComponent_SetType(EntityId, value);
		}

		public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake) 
			=> InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(EntityId, ref impulse, ref worldPosition, wake);
		
		public void ApplyLinearImpulse(Vector2 impulse, bool wake)
			=> InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(EntityId, ref impulse, wake);
	}

	public class TextComponent : Component
	{

		public string Text
		{
			get => InternalCalls.TextComponent_GetText(EntityId);
			set => InternalCalls.TextComponent_SetText(EntityId, value);
		}

		public Vector4 Color
		{
			get
			{
				InternalCalls.TextComponent_GetColor(EntityId, out Vector4 color);
				return color;
			}

			set => InternalCalls.TextComponent_SetColor(EntityId, ref value);
		}

		public float Kerning
		{
			get => InternalCalls.TextComponent_GetKerning(EntityId);
			set => InternalCalls.TextComponent_SetKerning(EntityId, value);
		}

		public float LineSpacing
		{
			get => InternalCalls.TextComponent_GetLineSpacing(EntityId);
			set => InternalCalls.TextComponent_SetLineSpacing(EntityId, value);
		}

	}

	public class CircleRendererComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.CircleRendererComponent_GetColor(EntityId, out Vector4 color);
				return color;
			}
			set => InternalCalls.CircleRendererComponent_SetColor(EntityId, ref value);
		}
		public float Thickness
		{
			get => InternalCalls.CircleRendererComponent_GetThickness(EntityId);
			set => InternalCalls.CircleRendererComponent_SetThickness(EntityId, value);
		}
		public float Fade
		{
			get => InternalCalls.CircleRendererComponent_GetFade(EntityId);
			set => InternalCalls.CircleRendererComponent_SetFade(EntityId, value);
		}
	}
}
