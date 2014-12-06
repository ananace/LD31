namespace Asteroids
{
	class Asteroid
	{
		Asteroid(Rect&in area, float baseRadius = 37.5f)
		{
			Radius = Math::Random(baseRadius * 0.75, baseRadius * 1.25);

			float ang = Math::Random(0.f, Math::DOUBLE_PI);

			RotationMult = Math::Random(-16.f, 16.f);
			Position = area.Constrain(Vec2(cos(ang) * 512, sin(ang) * 512));
			Position += Vec2(cos(ang), sin(ang)) * Radius;

			ang += Math::PI;
			Inertia = Vec2(cos(ang) * Math::Random(10.f, 20.f), sin(ang) * Math::Random(10.f, 20.f));

			float pointCount = baseRadius * 1.1;
			Shape = Shapes::Polygon(int(Math::Random(pointCount * 0.75, pointCount * 1.25)));

			for (uint i = 0; i < Shape.PointCount; ++i)
			{
				Vec2 pointPos(cos((Math::DOUBLE_PI / Shape.PointCount) * i),
					sin((Math::DOUBLE_PI / Shape.PointCount) * i));

				Shape.SetPoint(i, pointPos * Math::Random(Radius * 0.75, Radius * 1.25));
			}

			Shape.FillColor = Colors::Black;
			Shape.OutlineColor = Colors::White;
			Shape.OutlineThickness = 1;
		}

		void Draw(Renderer@ rend)
		{
			Shape.Position = Position;
			Shape.Rotation = Rotation;

			rend.Draw(Shape);
		}

		Shapes::Polygon Shape;

		float Radius;
		float Rotation, RotationMult;
		Vec2 Position;
		Vec2 Inertia;
	}

	class Bullet
	{
		void Draw(Renderer@ rend)
		{
			Shapes::Circle shape(Radius, 6);

			shape.FillColor = Colors::Transparent;
			shape.OutlineColor = Colors::Yellow;
			shape.OutlineThickness = 1;

			shape.SetOrigin(Radius, Radius);
			shape.Position = Position;

			rend.Draw(shape);
		}

		float Radius = 2.5;
		Vec2 Inertia;
		Vec2 Position;
	}

	class Ship
	{
		void Draw(Renderer@ rend)
		{
			if (Accel > 0)
			{
				Shapes::Polygon exhaust = {
					Vec2(10, 0),
					Vec2(10, 10),
					Vec2(3, 2),
					Vec2(0, 12),
					Vec2(-3, 1),
					Vec2(-10, 10),
					Vec2(-10, 0),
					Vec2(0, -5)
				};

				exhaust.SetPoint(1, Vec2(10 + sin(Anim * 2.15), 10));
				exhaust.SetPoint(2, Vec2(3, 2 + sin(Anim * 1.25)));
				exhaust.SetPoint(3, Vec2(sin(Anim * 1.75), 12));
				exhaust.SetPoint(4, Vec2(-3, 2 + cos(Anim)));
				exhaust.SetPoint(5, Vec2(-10 + cos(Anim * 2.25), 10));

				exhaust.SetOrigin(0, -15);
				exhaust.SetScale(Accel);

				exhaust.FillColor = Colors::Transparent;
				exhaust.OutlineColor = Color(16, 196, 255);
				exhaust.OutlineThickness = 1.5;

				exhaust.Position = Position;
				exhaust.Rotation = Rotation;

				rend.Draw(exhaust);
			}

			Shapes::Polygon ship = {
				Vec2(0, -20),
				Vec2(10, 10),
				Vec2(0, 5),
				Vec2(-10, 10)
			};

			ship.Position = Position;

			ship.FillColor = Colors::Black;
			ship.OutlineColor = Colors::White;
			ship.OutlineThickness = 2;

			ship.Rotation = Rotation;

			rend.Draw(ship);
		}

		float Accel;
		float Anim;

		float Rotation;
		Vec2 Position;
		Vec2 Inertia;
	}
}