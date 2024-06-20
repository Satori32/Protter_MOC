#include <iostream>

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>

class Surface24 {
public:
	struct RGB {
		std::uint8_t B = 0;
		std::uint8_t G = 0;
		std::uint8_t R = 0;
	};

	RGB& Index(const std::size_t& X, const std::size_t& Y) {

		if (X >= W) throw std::out_of_range("OutOfRange:surface24");
		if (Y >= H) throw std::out_of_range("OutOfRange:surface24");
		return D[W * Y + X];
	}
	const RGB& Index(const std::size_t& X, const std::size_t& Y)const  {

		if (X >= W) throw std::out_of_range("OutOfRange:surface24");
		if (Y >= H) throw std::out_of_range("OutOfRange:surface24");
		return D[W * Y + X];
	}
	std::size_t Width() const {
		return W;
	}
	std::size_t Height() const {
		return H;
	}
	bool Clear() {
		D.clear();
		return true;
	}
	bool Resize(const std::size_t Width, std::size_t Height) {
		D.clear();
		D.resize(Width * Height);
		W = Width;
		H = Height;
		return true;
	}

	bool SetPixel(const std::size_t& X,const std::size_t& Y,const RGB& C) {

		if (X >= Width()) { return false; }
		if (Y >= Height()) { return false; }

		Index(X, Y) = C;

		return true;
	}

	std::size_t CulcPod4() const {
		std::size_t X = (W*3) % 4;
		std::size_t Y = 3 - X;

		return X == 0 ? X : Y;
		//return 0;
	}

protected:
	std::vector<RGB> D{};
	std::size_t W = 0;
	std::size_t H = 0;
 };

struct BitMapFileHeader
{
	std::int16_t Type = 'B'+('M'<<8);
	std::int32_t Size = 54;//+colors
	std::int16_t Resurved1 = 0xcc;
	std::int16_t Resurved2 = 0xcc;
	std::int32_t OffBits = 0;
};

struct BitMapHeader
{
	std::int32_t Size = sizeof(BitMapHeader);
	std::int32_t Width = 0;
	std::int32_t Height = 0;
	std::int16_t Planes = 0;
	std::int16_t BitCount = 24;
	std::int32_t Compression = 0;//BI_RGB=0.
	std::int32_t Imazesize = 0;
	std::int32_t XPixelParMeter = 0;
	std::int32_t YPixelParMeter = 0;
	std::int32_t ColorUsed = 0;
	std::int32_t ColorImportant = 0;
};
std::size_t culcBitmapFileHeader() {
	BitMapFileHeader BFH;
	return sizeof(BFH.Type) + sizeof(BFH.Size) + sizeof(BFH.Resurved1) + sizeof(BFH.Resurved2) + sizeof(BFH.OffBits);
}
std::ofstream& WriteSurface24(std::ofstream& ofs,const Surface24& S) {
	for (std::intmax_t Y = S.Height()-1; Y >= 0; Y--) {
		for (std::size_t X = 0; X < S.Width(); X++) {
			ofs.write((char*)(&S.Index(X, Y)), 3);
		}
		for (std::size_t i = 0; i < S.CulcPod4(); i++) {
			char c = 0xcc;
			ofs.write(&c,1);
		}
	}
	return ofs;
}
BitMapFileHeader MakeBitmapFileHeader(const Surface24& S) {
	BitMapFileHeader BFH = {};
	BFH.Size =	BFH.Size+ S.Height() * ((S.Width() * 3));//+ S.CulcPod4());
	BFH.OffBits = culcBitmapFileHeader() + sizeof(BitMapHeader);

	//In.write((char*)&BFH, sizeof(BFH));
	return BFH;
}
BitMapHeader MakeBitmapHeader(const Surface24& S) {
	BitMapHeader BH = {};

	BH.Width = S.Width();
	BH.Height = S.Height();
	BH.BitCount = 24;
	BH.Imazesize = 0;// S.Height() * ((S.Width() * 3) + S.CulcPod4());

	return BH;
}
std::ofstream& WriteBitmapFileHeader(std::ofstream& ofs, const BitMapFileHeader& B) {
	ofs.write((char*)&B.Type, sizeof(B.Type));
	ofs.write((char*)&B.Size, sizeof(B.Size));
	ofs.write((char*)&B.Resurved1, sizeof(B.Resurved1));
	ofs.write((char*)&B.Resurved2, sizeof(B.Resurved2));
	ofs.write((char*)&B.OffBits, sizeof(B.OffBits));
	//ofs.write((char*)&B, sizeof(B));
	return ofs;
}
std::ofstream& WriteBitmapHeader(std::ofstream& ofs, const BitMapHeader& B) {
	ofs.write((char*)&B, sizeof(B));
	return ofs;
}
/** /
int main() {
	Surface24 S;


	std::string Name = "Hoge.bmp";

	S.Resize(256, 256);

	BitMapFileHeader BHF = MakeBitmapFileHeader(S);
	BitMapHeader BH =MakeBitmapHeader(S);

	auto X = S.CulcPod4();
	S.Index(0, 0).R = 100;
	{
		std::ofstream ofs(Name, std::ios::binary);

		WriteBitmapFileHeader(ofs, BHF);
		WriteBitmapHeader(ofs, BH);
		WriteSurface24(ofs, S);
	}
	return 0;
}/**/

//y = lambda;

template<class F>
Surface24 Prot(double XF, double Delta, F P) {
	Surface24 S;
	S.Resize(256, 256);
	double M = 1 / Delta;
	double X = XF;
	for (std::intmax_t i = 0; M * i <= 1; i++) {
		double Y = P(X+i);
		S.SetPixel(X+i, Y, { 0xff,0xff,0xff });
	}

	return S;

}
/** /
int main() {

	//auto F = [](auto X) {return X + 1; };
	auto S = Prot(0, 100,[](auto X) {return X*X; });
	std::string Name = "Test.bmp";


	BitMapFileHeader BHF = MakeBitmapFileHeader(S);
	BitMapHeader BH = MakeBitmapHeader(S);
	{
		std::ofstream ofs(Name, std::ios::binary);

		WriteBitmapFileHeader(ofs, BHF);
		WriteBitmapHeader(ofs, BH);
		WriteSurface24(ofs, S);
	}


	return 0;
}/**/
class Plotter {
public:
	struct Point2D
	{
		std::intmax_t X=0;
		std::intmax_t Y=0;
	};

	bool SetOrigin(const Point2D& In) {//Absolute position.
		Origin = In;
		return true;
	}
	bool SetOrignRelative(const Point2D& In) {
		Origin.X = +In.X;
		Origin.Y = +In.Y;
		return true;
	}
	bool SetOriginCaptured(const Point2D& In) {
		Origin.X += XPlusToUp ? In.X : -In.X;
		Origin.Y += YPlusToUP ? In.Y : -In.Y;
		return true;
	}

	bool SetYPlusIsUp(bool B) {
		YPlusToUP = B;
		return true;
	}
	bool SetXPlusToUp(bool B) {
		XPlusToUp = B;
		return true;
	}

	bool SetPixel(const Point2D& In, Surface24::RGB C) {

		Point2D P = Origin;
		P.X += XPlusToUp ? In.X : -In.X;
		P.Y += YPlusToUP ? In.Y : -In.Y;

		S.SetPixel(P.X, P.Y,C);

		return true;
	}

	const Surface24& GetSurface() {
		return S;
	}
	bool ResizeSurface(std::size_t W, std::size_t H) {
		S.Resize(W, H);
		return true;
	}

protected:
	Surface24 S;
	Point2D Origin;
	bool YPlusToUP = false;
	bool XPlusToUp = true;
};

int main() {
	Plotter P;
	P.SetOrigin({ 128, 128 });
	P.SetYPlusIsUp(true);
	P.SetXPlusToUp(false);


	P.ResizeSurface(256, 256);

	auto F = [](auto X) {return X; };//Y=Lambda.

	for (std::size_t i = 0; i < 100; i++) {
		std::intmax_t Y = F(i);
		P.SetPixel({ (std::intmax_t)i,Y }, {0,0,0xff});
	}
	{
		std::string Name = "Test.bmp";
		auto& S = P.GetSurface();

		BitMapFileHeader BHF = MakeBitmapFileHeader(S);
		BitMapHeader BH = MakeBitmapHeader(S);

		{
			std::ofstream ofs(Name, std::ios::binary);

			WriteBitmapFileHeader(ofs, BHF);
			WriteBitmapHeader(ofs, BH);
			WriteSurface24(ofs, S);
		}

	}
}