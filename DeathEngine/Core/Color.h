
#ifndef _COLOR_H_

#define _COLOR_H_

namespace Core
{

class Color24
{
	public:
	
		Color24() {};
		Color24(const byte _r, const byte _g, const byte _b) { r=_r; g=_g; b=_b; };
		Color24(const Color24& c) { r=c.r; g=c.g; b=c.b; };
		Color24(const byte *pc) { r=pc[0]; g=pc[1]; b=pc[2]; };
		~Color24() {};
		
		const Color24& operator=(const Color24& c) { r=c.r; g=c.g; b=c.b; return *this; };
		const Color24& operator=(const byte *pc) { r=pc[0]; g=pc[1]; b=pc[2]; return *this; };

		bool operator==(const Color24& c) const { return (r==c.r) && (g==c.g) && (b==c.b); };
		bool operator!=(const Color24& c) const { return (r!=c.r) || (g!=c.g) || (b!=c.b); };

		Color24 operator+(const Color24& c) const { return Color24(r+c.r, g+c.g, b+c.b); };
		const Color24& operator+=(const Color24& c) { r+=c.r; g+=c.g; b+=c.b; return *this; };

		Color24 operator-(const Color24& c) const { return Color24(r-c.r, g-c.g, b-c.b); };
		const Color24& operator-=(const Color24& c) { r-=c.r; g-=c.g; b-=c.b; return *this; };

		Color24 operator*(const byte a) const { return Color24((r * a) >> 8, (g*a) >> 8, (b*a) >> 8); };
		const Color24& operator*=(const byte a) { r=(r*a) >> 8; g=(g*a) >> 8; b=(b*a) >> 8; return *this; };

		Color24 operator*(const float f) const { return Color24((byte)(((float)r)*f), (byte)(((float)g)*f), (byte)(((float)b)*f)); };
		const Color24& operator*=(const float f) { r=(byte)(((float)r)*f); g=(byte)(((float)g)*f); b=(byte)(((float)b)*f); return *this; };
		
		uint32 ToRGB() const { return ((r << 16) | (g << 8) | b); };

		byte r,g,b;

};

class Color32
{

	public:
	
		Color32() {};
		Color32(const byte _a, const byte _r, const byte _g, const byte _b) { a=_a; r=_r; g=_g; b=_b; };
		Color32(const byte *pc) { a=pc[0]; r=pc[1]; g=pc[2]; b=pc[3]; };
		Color32(const Color32& c) { dwRGB=c.dwRGB; };
		~Color32() {};
		
		const Color32& operator=(const Color32& c) { dwRGB=c.dwRGB; return *this; };
		const Color32& operator=(const byte *pc) { a=pc[0]; r=pc[1]; g=pc[2]; b=pc[3]; return *this; };
		const Color32& operator=(const uint32 _RGB) { dwRGB=_RGB; return *this; };

		bool operator==(const Color32& c) const { return dwRGB==c.dwRGB; };
		bool operator!=(const Color32& c) const { return dwRGB!=c.dwRGB; };

		Color32 operator+(const Color32& c) const { return Color32(a+c.a,r+c.r, g+c.g, b+c.b); };
		const Color32& operator+=(const Color32& c) { a+=c.a; r+=c.r; g+=c.g; b+=c.b; return *this; };

		Color32 operator-(const Color32& c) const { return Color32(a-c.a,r-c.r, g-c.g, b-c.b); };
		const Color32& operator-=(const Color32& c) { a-=c.a; r-=c.r; g-=c.g; b-=c.b; return *this; };

		Color32 operator*(const byte _a) const { return Color32((a * _a) >> 8, (r * _a) >> 8, (g*_a) >> 8, (b*_a) >> 8); };
		const Color32& operator*=(const byte _a) { a=(a*_a) >> 8; r=(r*_a) >> 8; g=(g*_a) >> 8; b=(b*_a) >> 8; return *this; };

		Color32 operator*(const float f) const { return Color32((byte)(((float)a)*f), (byte)(((float)r)*f), (byte)(((float)g)*f), (byte)(((float)b)*f)); };
		const Color32& operator*=(const float f) { a=(byte)(((float)r)*f); r=(byte)(((float)r)*f); g=(byte)(((float)g)*f); b=(byte)(((float)b)*f); return *this; };
		
		uint32 ToRGB() const { return dwRGB; };

		Color32 GetInterpolated(const Color32 &other, float d) const
		{
			const float inv = 1.0f - d;
			return Color32((byte)(other.a*inv + a*d),
				(byte)(other.r*inv + r*d),
				(byte)(other.g*inv + g*d),
				(byte)(other.b*inv + b*d));
		}

		union
		{
			struct
			{
				byte b,g,r,a;
			};
			uint32 dwRGB;
		};

};

class Color24f
{
	public:
	
		Color24f() {};
		Color24f(const float _r, const float _g, const float _b) { fr=_r; fg=_g; fb=_b; };
		Color24f(const Color24f& c) { fr=c.fr; fg=c.fg; fb=c.fb; };
		Color24f(const float *pc) { fr=pc[0]; fg=pc[1]; fb=pc[2]; };
		Color24f(const Color24& c) { fr=(float)c.r / 255.0f; fg=(float)c.g / 255.0f; fb=(float)c.b / 255.0f; };
		~Color24f() {};
		
		const Color24f& operator=(const Color24f& c) { fr=c.fr; fg=c.fg; fb=c.fb; return *this; };
		const Color24f& operator=(const Color24& c) { fr=(float)c.r / 255.0f; fg=(float)c.g / 255.0f; fb=(float)c.b / 255.0f; return *this; };
		const Color24f& operator=(const float *pc) { fr=pc[0]; fg=pc[1]; fb=pc[2]; return *this; };

		bool operator==(const Color24f& c) const { return (fr==c.fr) && (fg==c.fg) && (fb==c.fb); };
		bool operator!=(const Color24f& c) const { return (fr!=c.fr) || (fg!=c.fg) || (fb!=c.fb); };

		Color24f operator+(const Color24f& c) const { return Color24f(fr+c.fr, fg+c.fg, fb+c.fb); };
		const Color24f& operator+=(const Color24f& c) { fr+=c.fr; fg+=c.fg; fb+=c.fb; return *this; };

		Color24f operator-(const Color24f& c) const { return Color24f(fr-c.fr, fg-c.fg, fb-c.fb); };
		const Color24f& operator-=(const Color24f& c) { fr-=c.fr; fg-=c.fg; fb-=c.fb; return *this; };

		Color24f operator*(const float f) const { return Color24f(fr*f,fg*f,fb*f); };
		const Color24f& operator*=(const float f) { fr*=f; fg*=f; fb*=f; return *this; };
		
		uint32 ToRGB() const { return (((uint32)(fr*255.0f)) << 16 | ((uint32)(fg*255.0f)) << 8 | ((uint32)(fb*255.0f))); };
		
		float fr,fg,fb;

};

class Color32f
{
	public:
	
		Color32f() {};
		Color32f(const float _a,const float _r, const float _g, const float _b) { fa=_a; fr=_r; fg=_g; fb=_b; };
		Color32f(const Color32f& c) { fa=c.fa; fr=c.fr; fg=c.fg; fb=c.fb; };
		Color32f(const Color32& c) { fa=(float)c.a / 255.0f; fr=(float)c.r / 255.0f; fg=(float)c.g / 255.0f; fb=(float)c.b / 255.0f; };
		Color32f(const float *pc) { fa=pc[0]; fr=pc[1]; fg=pc[2]; fb=pc[3]; };
		~Color32f() {};
		
		const Color32f& operator=(const Color32f& c) { fa=c.fa; fr=c.fr; fg=c.fg; fb=c.fb; return *this; };
		const Color32f& operator=(const Color32& c) { fa=(float)c.a / 255.0f; fr=(float)c.r / 255.0f; fg=(float)c.g / 255.0f; fb=(float)c.b / 255.0f; return *this; };
		const Color32f& operator=(const float *pc) { fa=pc[0]; fr=pc[1]; fg=pc[2]; fb=pc[3]; return *this; };

		bool operator==(const Color32f& c) const { return (fa==c.fa) && (fr==c.fr) && (fg==c.fg) && (fb==c.fb); };
		bool operator!=(const Color32f& c) const { return (fa!=c.fa) || (fr!=c.fr) || (fg!=c.fg) || (fb!=c.fb); };

		Color32f operator+(const Color32f& c) const { return Color32f(fa+c.fa, fr+c.fr, fg+c.fg, fb+c.fb); };
		const Color32f& operator+=(const Color32f& c) { fa+=c.fa; fr+=c.fr; fg+=c.fg; fb+=c.fb; return *this; };

		Color32f operator-(const Color32f& c) const { return Color32f(fa-c.fa, fr-c.fr, fg-c.fg, fb-c.fb); };
		const Color32f& operator-=(const Color32f& c) { fa-=c.fa; fr-=c.fr; fg-=c.fg; fb-=c.fb; return *this; };

		Color32f operator*(const float f) const { return Color32f(fa*f,fr*f,fg*f,fb*f); };
		const Color32f& operator*=(const float f) { fa*=f; fr*=f; fg*=f; fb*=f; return *this; };
		
		uint32 ToRGB() const { return (((uint32)fa) << 24 | ((uint32)fr) << 16 | ((uint32)fg) << 8 | ((uint32)fb)); };
		
		float fa,fr,fg,fb;

};

class ColorHSL
{
	public:

		ColorHSL(float h=0.0f, float s=0.0f, float l=0.0f) : hue(h),saturation(s),luminance(l) {}

		void ToRGB(Color32& color)
		{
			if(saturation==0.0f)
			{
				byte c=(byte)(luminance * 255.0f);
				color.r=color.g=color.b=c;
				return;
			}

			float rm1,rm2;

			if(luminance <= 0.5f)
				rm2=luminance + luminance * saturation;
			else
				rm2=luminance + saturation - luminance * saturation;

			rm1=2.0f * luminance - rm2;

			color.r=(byte)ToRGB1(rm1,rm2,hue + (120.0f * DEGTORAD));
			color.g=(byte)ToRGB1(rm1,rm2,hue);
			color.b=(byte)ToRGB1(rm1,rm2,hue - (120.0f * DEGTORAD));
		}

		float hue,saturation,luminance;

	protected:

		uint32 ToRGB1(float rm1, float rm2, float rh) const
		{
			while(rh > 2.0f * PI) rh-=2.0f * PI;
			while(rh < 0.0f) rh+=2.0f * PI;

			if     (rh <  60.0f * DEGTORAD) rm1=rm1 + (rm2 - rm1) * rh / (60.0f * DEGTORAD);
			else if(rh < 180.0f * DEGTORAD) rm1=rm2;
			else if(rh < 240.0f * DEGTORAD) rm1=rm1 + (rm2 - rm1) * ((240.0f * DEGTORAD) - rh) / (60.0f * DEGTORAD);

			return (uint32)(rm1 * 255.0f);
		}
};

}

#endif