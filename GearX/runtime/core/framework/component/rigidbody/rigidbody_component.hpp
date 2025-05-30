#pragma once
#include "../component.hpp"
constexpr float MIN_FORC = 0.0f;
constexpr float MIN_ANGLE = 0.0f;
constexpr float MAX_ANGLE = M_PI * 2;
// �ٶ�����
constexpr float MAX_LINEAR_VELOCITY = 100.0f;
constexpr float MAX_ANGULAR_VELOCITY = M_PI * 10;
// ��������
constexpr float MIN_DAMPING = 0.0f;
constexpr float MAX_DAMPING = 10.0f;
// �ܶ�����
constexpr float MIN_DENSITY = 0.0f;
constexpr float MAX_DENSITY = 100.0f;
// Ħ��ϵ������
constexpr float MIN_FRICTION = 0.0f;
constexpr float MAX_FRICTION = 1.0f;
// ����ϵ������
constexpr float MIN_RESTITUTION = 0.0f;
constexpr float MAX_RESTITUTION = 1.0f;
// ������������
constexpr float MIN_GRAVITY_SCALE = -10.0f;
constexpr float MAX_GRAVITY_SCALE = 10.0f;
// ������ֵ����
constexpr float MIN_RESTITUTION_THRESHOLD = 0.0f;
constexpr float MAX_RESTITUTION_THRESHOLD = 10.0f;
// ����/�ױ�����100����=1�ף�
constexpr float PPM = 100;

namespace GearX {
	class Component;
	class GObject;
	float clamp(float value, float min, float max);
	//��ײ������
	enum class BoxType{
		Rectangle,
		circle
	};
	//��ײ���趨ģʽ
	enum class BoxMode {
		Auto,//�Զ�������ײ���С������TextureComponent����͸������
		Inherit,//�̳�TextureComponent�Ĵ�С
		Custom //�Զ����С
	};
	class RigidBodyComponent : public Component {
	public:
		RigidBodyComponent() {
			m_type_name = "RigidBodyComponent";
			m_body = nullptr;
			m_fixture = nullptr;
			m_fixtureDef = b2FixtureDef();
			m_bodyDef = b2BodyDef();
		}
		void init();
		void destroy();
		// �Ƕ����ƣ����ȣ�
		void updateTransform();
		std::shared_ptr<Component> copy();
		void setAllowSleep(bool ap) { m_bodyDef.allowSleep = ap; if (m_body) m_body->SetSleepingAllowed(ap); }
		bool getAlllowSleep() const { return m_bodyDef.allowSleep; }
		void setAwake(bool awake) { m_bodyDef.awake = awake; if (m_body) m_body->SetAwake(awake); }
		bool getAwake() const { return m_bodyDef.awake; }
		void setBullet(bool bullet) { m_bodyDef.bullet = bullet; if (m_body) m_body->SetBullet(bullet); }
		bool getBullet() const { return m_bodyDef.bullet; }
		void setAngularDamping(float angularDamping) {
			angularDamping = clamp(angularDamping, MIN_DAMPING, MAX_DAMPING);
			m_bodyDef.angularDamping = angularDamping;
			if (m_body) m_body->SetAngularDamping(angularDamping);
		}
		float getAngularDamping() const { return m_bodyDef.angularDamping; }
		void setAngularVelocity(float angularVelocity) {
			angularVelocity = clamp(angularVelocity, -MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);
			m_bodyDef.angularVelocity = angularVelocity;
			if (m_body) m_body->SetAngularVelocity(angularVelocity);
		}
		float getAngularVelocity() const { return m_bodyDef.angularVelocity; }
		void setLinearDamping(float linearDamping) {
			linearDamping = clamp(linearDamping, MIN_DAMPING, MAX_DAMPING);
			m_bodyDef.linearDamping = linearDamping;
			if (m_body) m_body->SetLinearDamping(linearDamping);
		}
		float getLinearDamping() const { return m_bodyDef.linearDamping; }
		void setLinearVelocity(std::array<float, 2> linearVelocity) {
			float vx = clamp(linearVelocity[0], -MAX_LINEAR_VELOCITY, MAX_LINEAR_VELOCITY);
			float vy = clamp(linearVelocity[1], -MAX_LINEAR_VELOCITY, MAX_LINEAR_VELOCITY);
			m_bodyDef.linearVelocity = b2Vec2(vx, vy);
			if (m_body) m_body->SetLinearVelocity(m_bodyDef.linearVelocity);
		}
		std::array<float, 2> getLinearVelocity() const {
			return { m_bodyDef.linearVelocity.x, m_bodyDef.linearVelocity.y };
		}
		void setDensity(float density) {
			density = clamp(density, MIN_DENSITY, MAX_DENSITY);
			m_fixtureDef.density = density;
			if (m_fixture) m_fixture->SetDensity(density);
		}
		float getDensity() const { return m_fixtureDef.density; }
		void setFriction(float friction) {
			friction = clamp(friction, MIN_FRICTION, MAX_FRICTION);
			m_fixtureDef.friction = friction;
			if (m_fixture) m_fixture->SetFriction(friction);
		}
		float getFriction() const { return m_fixtureDef.friction; }
		void setRestitution(float restitution) {
			restitution = clamp(restitution, MIN_RESTITUTION, MAX_RESTITUTION);
			m_fixtureDef.restitution = restitution;
			if (m_fixture) m_fixture->SetRestitution(restitution);
		}
		float getRestitution() const { return m_fixtureDef.restitution; }
		void setSensor(bool sensor) { m_fixtureDef.isSensor = sensor; if (m_fixture) m_fixture->SetSensor(sensor); }
		bool isSensor() const { return m_fixtureDef.isSensor; }
		void setRestitutionThreshold(float restitutionThreshold) {
			restitutionThreshold = clamp(restitutionThreshold, MIN_RESTITUTION_THRESHOLD, MAX_RESTITUTION_THRESHOLD);
			m_fixtureDef.restitutionThreshold = restitutionThreshold;
			if (m_fixture) m_fixture->SetRestitutionThreshold(restitutionThreshold);
		}
		float getRestitutionThreshold() const { return m_fixtureDef.restitutionThreshold; }
		// ������ getter �� setter
		void setFixedRotation(bool fixedRotation) { m_bodyDef.fixedRotation = fixedRotation; if (m_body) m_body->SetFixedRotation(fixedRotation); }
		bool getFixedRotation() const { return m_bodyDef.fixedRotation; }
		void setGravityScale(float gravityScale) {
			gravityScale = clamp(gravityScale, MIN_GRAVITY_SCALE, MAX_GRAVITY_SCALE);
			m_bodyDef.gravityScale = gravityScale;
			if (m_body) m_body->SetGravityScale(gravityScale);
		}
		float getGravityScale() const { return m_bodyDef.gravityScale; }
		void setAngle(float angle) {
			if (angle > MAX_ANGLE) {
                angle = angle - MAX_ANGLE;
			}
			if (angle < MIN_ANGLE) {
                angle = angle + MAX_ANGLE;
			}
			m_bodyDef.angle = angle;
			if (m_body) m_body->SetTransform(m_body->GetPosition(), angle); m_isDirty = true;
		}
		float getAngle() const { return m_bodyDef.angle; }
		void setPosition(std::array<float, 2> pos) {
			m_bodyDef.position.Set(pos[0], pos[1]);
			if (m_body) m_body->SetTransform(m_bodyDef.position, m_body->GetAngle()); m_isDirty = true;
		}
		std::array<float, 2> getPosition() const { return { m_bodyDef.position.x, m_bodyDef.position.y }; }
		void setBodyType(b2BodyType type) {
			// ȷ��������Ч
			if (type >= b2_staticBody && type <= b2_dynamicBody) {
				m_bodyDef.type = type;
				if (m_body) m_body->SetType(type);
			}
		}
		b2BodyType getBodyType() const { return m_bodyDef.type; }

		// ��ȡ�ײ�Box2D����ķ���
		b2Body* getBody() const { return m_body; }
		b2Fixture* getFixture() {
				return m_fixture;
		}
		// ��ʼ������ - ͨ���������ӵ�ʵ������
		void initialize(b2World* world) {
			if (!m_body && world) {
				m_body = world->CreateBody(&m_bodyDef);
				m_fixture = m_body->CreateFixture(&m_fixtureDef);
			}
		}

		// ������ - ͨ���������ʵ���Ƴ�ǰ����
		void cleanup(b2World* world) {
			if (m_body && world) {
				world->DestroyBody(m_body);
				m_body = nullptr;
				m_fixture = nullptr;
			}
		}
		void applyForce(std::array<float, 2> force) {
			if (m_body) {
				b2Vec2 b2Force(force[0], force[1]);
				m_body->ApplyForce(b2Force, m_body->GetWorldCenter(), true);
			}
		}
		void applyImpulse(std::array<float, 2> impulse) {
			if (m_body) {
				b2Vec2 b2Impulse(impulse[0], impulse[1]);
				m_body->ApplyLinearImpulse(b2Impulse, m_body->GetWorldCenter(), true);
			}
		}
		void applyTorque(float torque) {
			if (m_body) {
				m_body->ApplyTorque(torque, true);
			}
		}
		void applyAngularImpulse(float impulse) {
			if (m_body) {
				m_body->ApplyAngularImpulse(impulse, true);
			}
		}
		void applyForceToCenter(std::array<float, 2> force) {
			if (m_body) {
				b2Vec2 b2Force(force[0], force[1]);
				m_body->ApplyForceToCenter(b2Force, true);
			}
		}
		void applyImpulseToCenter(std::array<float, 2> impulse) {
			if (m_body) {
				b2Vec2 b2Impulse(impulse[0], impulse[1]);
				m_body->ApplyLinearImpulseToCenter(b2Impulse, true);
			}
		}

		static void addComponentTo(std::shared_ptr<GObject> obj);
		template<class Archive>
		void serialize(Archive& archive) {
			Component::serialize(archive);
			int type = int(m_bodyDef.type);
			using cereal::make_nvp;
			archive(
				make_nvp("bodyDef.allowSleep", m_bodyDef.allowSleep),
				make_nvp("bodyDef.angle", m_bodyDef.angle),
				make_nvp("bodyDef.angularDamping", m_bodyDef.angularDamping),
				make_nvp("bodyDef.angularVelocity", m_bodyDef.angularVelocity),
				make_nvp("bodyDef.awake", m_bodyDef.awake),
				make_nvp("bodyDef.bullet", m_bodyDef.bullet),
				make_nvp("bodyDef.fixedRotation", m_bodyDef.fixedRotation),
				make_nvp("bodyDef.gravityScale", m_bodyDef.gravityScale),
				make_nvp("bodyDef.linearDamping", m_bodyDef.linearDamping),
				make_nvp("bodyDef.linearVelocity.x", m_bodyDef.linearVelocity.x),
				make_nvp("bodyDef.linearVelocity.y", m_bodyDef.linearVelocity.y),
				make_nvp("bodyDef.position.x", m_bodyDef.position.x),
				make_nvp("bodyDef.position.y", m_bodyDef.position.y),
				make_nvp("bodyDef.type", type),
				make_nvp("fixture.density", m_fixtureDef.density),
				make_nvp("fixture.friction", m_fixtureDef.friction),
				make_nvp("fixture.restitution", m_fixtureDef.restitution),
				make_nvp("fixture.isSensor", m_fixtureDef.isSensor),
				make_nvp("fixture.restitutionThreshold", m_fixtureDef.restitutionThreshold)
			);
			
			m_bodyDef.type = (b2BodyType)(type);
		}
	private:
		b2Body* m_body;
		b2BodyDef m_bodyDef;
		b2Fixture* m_fixture;
		b2FixtureDef m_fixtureDef;
		RTTR_ENABLE(Component)
			RTTR_REGISTRATION_FRIEND
	};
}
CEREAL_REGISTER_TYPE(GearX::RigidBodyComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(GearX::Component, GearX::RigidBodyComponent)