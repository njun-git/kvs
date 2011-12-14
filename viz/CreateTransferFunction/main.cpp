//
//  main.cpp
//  
//
//  Created by njun on 11/25/7.
//  Copyright 2011 Jun Nishimura. All rights reserved.
//

#include <kvs/CommandLine>
#include <kvs/TransferFunction>
#include <kvs/RGBFormulae>

#include <cmath>

namespace
{

double deg2rad( double d ) { return( d * 3.141592 / 180.0 ); }

double f00( double x ) { return( 0 ); }
double f01( double x ) { return( 0.5 ); }
double f02( double x ) { return( 1.0 ); }
double f03( double x ) { return( x ); }
double f04( double x ) { return( x * x ); }
double f05( double x ) { return( x * x * x ); }
double f06( double x ) { return( x * x * x * x ); }
double f07( double x ) { return( std::sqrt(x) ); }
double f08( double x ) { return( std::sqrt( std::sqrt(x) ) ); }
double f09( double x ) { return( std::sin( deg2rad( 90.0 * x ) ) ); }
double f10( double x ) { return( std::cos( deg2rad( 90.0 * x ) ) ); }
double f11( double x ) { return( std::abs( x - 0.5 ) ); }
double f12( double x ) { return( ( 2.0 * x - 1.0 ) * ( 2.0 * x - 1.0 ) ); }
double f13( double x ) { return( std::sin( deg2rad( 180.0 * x ) ) ); }
double f14( double x ) { return( std::abs( std::cos( deg2rad( 180.0 * x ) ) ) ); }
double f15( double x ) { return( std::sin( deg2rad( 360.0 * x ) ) ); }
double f16( double x ) { return( std::cos( deg2rad( 360.0 * x ) ) ); }
double f17( double x ) { return( std::abs( std::sin( deg2rad( 360.0 * x ) ) ) ); }
double f18( double x ) { return( std::abs( std::cos( deg2rad( 360.0 * x ) ) ) ); }
double f19( double x ) { return( std::abs( std::sin( deg2rad( 720.0 * x ) ) ) ); }
double f20( double x ) { return( std::abs( std::cos( deg2rad( 720.0 * x ) ) ) ); }
double f21( double x ) { return( 3.0 * x ); }
double f22( double x ) { return( 3.0 * x - 1.0 ); }
double f23( double x ) { return( 3.0 * x - 2.0 ); }
double f24( double x ) { return( std::abs( 3.0 * x - 1.0 ) ); }
double f25( double x ) { return( std::abs( 3.0 * x - 2.0 ) ); }
double f26( double x ) { return( ( 3.0 * x - 1.0 ) / 2.0 ); }
double f27( double x ) { return( ( 3.0 * x - 2.0 ) / 2.0 ); }
double f28( double x ) { return( std::abs( ( 3.0 * x - 1.0 ) / 2.0 ) ); }
double f29( double x ) { return( std::abs( ( 3.0 * x - 2.0 ) / 2.0 ) ); }
double f30( double x ) { return( x / 0.32 - 0.78125 ); }
double f31( double x ) { return( 2.0 * x - 0.84 ); }
double f32( double x ) { return( x < 0.25 ? 4.0 * x : x < 0.50 ? 1.0 : x < 0.75 ? -2.0 * x + 1.84 : x / 0.08 - 11.5 ); }
double f33( double x ) { return( std::abs( 2.0f * x - 0.5 ) ); }
double f34( double x ) { return( 2.0 * x ); }
double f35( double x ) { return( 2.0 * x - 0.5 ); }
double f36( double x ) { return( 2.0 * x - 1.0 ); }
double f37( double x ) { return( x < 3.0/8 ? 0.0 : x < 5.0/8 ? 4.0 * x - 1.5 : x < 7.0/8 ? 1.0 : -4.0 * x + 4.5 ); }
double f38( double x ) { return( x < 1.0/8 ? 0.0 : x < 3.0/8 ? 4.0 * x - 0.5 : x < 5.0/8 ? 1.0 : x < 7.0/8 ? -4.0 * x + 3.5 : 0.0 ); }
double f39( double x ) { return( x < 1.0/8 ? 4.0 * x + 0.5 : x < 3.0/8 ? 1.0 : x < 5.0/8 ? -4.0 * x + 2.5 : 0.0 ); }
double f40( double x ) { return( x < 3.0/4 ? 7.0/8 * x : 11.0/8 * x - 3.0/8 ); }
double f41( double x ) { return( x < 3.0/8 ? 7.0/8 * x : x < 3.0/4 ? 29.0/24 * x - 1.0/8 : 7.0/8 * x + 1.0/8 ); }
double f42( double x ) { return( x < 3.0/8 ? 29.0/24 * x : 7.0/8 * x + 1.0/8 ); }

double (*f[])(double) = {
    f00, f01, f02, f03, f04, f05, f06, f07, f08, f09,
    f10, f11, f12, f13, f14, f15, f16, f17, f18, f19,
    f20, f21, f22, f23, f24, f25, f26, f27, f28, f29,
    f30, f31, f32, f33, f34, f35, f36, f37, f38, f39,
    f40, f41, f42
};

}

class Argument : public kvs::CommandLine
{

public:

    Argument( int argc, char** argv ) :
        CommandLine( argc, argv )
    {
        add_help_option();

        add_option( "file", "Output file name. ( default : tfunc.kvsml )", 1, false );

        add_option( "resolution", "[size_t] Resolution of transfer function. ( default : 256 )\n", 1, false );

        add_option( "PM3D", "[ -rgb 7 5 15 ]. ( optional )", 0, false );
        add_option( "GreenRedViolet", "[ -rgb 3 11 6 ]. ( optional )", 0, false );
        add_option( "Ocean", "[ -rgb 23 28 3 ]. ( optional )", 0, false );
        add_option( "Hot", "[ -rgb 21 22 23 ]. ( optional )", 0, false );
        add_option( "ColorPrintable", "[ -rgb 30 31 32 ]. ( optional )", 0, false );
        add_option( "Rainbow", "[ -rgb 33 13 10 ]. ( optional )", 0, false );
        add_option( "AFMHot", "[ -rgb 34 35 36 ]. ( optional )", 0, false );
        add_option( "Jet", "[ -rgb 37 38 39 ]. ( optional )", 0, false );
        add_option( "Bone", "[ -rgb 40 41 42 ]. ( optional )\n", 0, false );

        add_option( "rgb", "[int] Type of color map. ( optional )", 3, false );
        add_option( "opacity", "[int] Type of opacity map. ( default : 3 )\n", 1, false );

        add_option( "typeinfo", "Show type info of -rgb -opacity option.\n", 0, false );

        if( !this->parse() ) exit( EXIT_FAILURE );
    }
};

int main( int argc, char** argv )
{
    Argument arg( argc, argv );

    if ( arg.hasOption( "typeinfo" ) )
    {
        const char* help_message =
            "<About Type of -rgb -opacity Option>\n"
            " 0 : 0.0\n"
            " 1 : 0.5\n"
            " 2 : 1.0\n"
            " 3 : x\n"
            " 4 : x ^ 2\n"
            " 5 : x ^ 3\n"
            " 6 : x ^ 4\n"
            " 7 : sqrt( x )\n"
            " 8 : sqrt( sqrt( x ) )\n"
            " 9 : sin( sqrt( deg2rad( 90 * x ) ) )\n"
            "10 : cos( sqrt( deg2rad( 90 * x ) ) )\n"
            "11 : abs( x - 0.5 )\n"
            "12 : ( 2 * x - 1.0 ) ^ 2\n"
            "13 : sin( deg2rad( 180 * x ) ) )\n"
            "14 : abs( cos( deg2rad( 180 * x ) ) )\n"
            "15 : sin( deg2rad( 360 * x ) )\n"
            "16 : cos( deg2rad( 360 * x ) )\n"
            "17 : abs( sin( deg2rad( 360 * x ) ) )\n"
            "18 : abs( cos( deg2rad( 360 * x ) ) )\n"
            "19 : abs( sin( deg2rad( 720 * x ) ) )\n"
            "20 : abs( cos( deg2rad( 720 * x ) ) )\n"
            "21 : 3.0 * x\n"
            "22 : 3.0 * x - 1.0\n"
            "23 : 3.0 * x - 2.0\n"
            "24 : abs( 3.0 * x - 1.0 )\n"
            "25 : abs( 3.0 * x - 2.0 )\n"
            "26 : ( 3.0 * x - 1.0 ) / 2\n"
            "27 : ( 3.0 * x - 2.0 ) / 2\n"
            "28 : abs( ( 3.0 * x - 1.0 ) / 2 )\n"
            "29 : abs( ( 3.0 * x - 2.0 ) / 2 )\n"
            "30 : x / 0.32 - 0.78125\n"
            "31 : 2.0 * x - 0.84\n"
            "32 : x < 0.25 ? 4.0 * x : x < 0.50 ? 1.0 : x < 0.75 ? -2.0 * x + 1.84 : x / 0.08 - 11.5\n"
            "33 : abs( 2.0f * x - 0.5 )\n"
            "34 : 2.0 * x\n"
            "35 : 2.0 * x - 0.5\n"
            "36 : 2.0 * x - 1.0\n"
            "37 : x < 3.0/8 ? 0.0 : x < 5.0/8 ? 4.0 * x - 1.5 : x < 7.0/8 ? 1.0 : -4.0 * x + 4.5\n"
            "38 : x < 1.0/8 ? 0.0 : x < 3.0/8 ? 4.0 * x - 0.5 : x < 5.0/8 ? 1.0 : x < 7.0/8 ? -4.0 * x + 3.5 : 0.0\n"
            "39 : x < 1.0/8 ? 4.0 * x + 0.5 : x < 3.0/8 ? 1.0 : x < 5.0/8 ? -4.0 * x + 2.5 : 0.0\n"
            "40 : x < 3.0/4 ? 7.0/8 * x : 11.0/8 * x - 3.0/8\n"
            "41 : x < 3.0/8 ? 7.0/8 * x : x < 3.0/4 ? 29.0/24 * x - 1.0/8 : 7.0/8 * x + 1.0/8\n"
            "42 : x < 3.0/8 ? 29.0/24 * x : 7.0/8 * x + 1.0/8";

        std::cout << help_message << std::endl;

        return( 0 );
    }

    const std::string filename = arg.hasOption( "file" ) ? arg.optionValue<std::string>( "file" ) : "tfunc.kvsml";
    const size_t resolution = arg.hasOption( "resolution" ) ? arg.optionValue<size_t>( "resolution" ) : 256;

    kvs::ColorMap color_map( resolution );
    if ( arg.hasOption( "rgb" ) )
    {
        const int r = arg.optionValue<int>( "rgb", 0 );
        const int g = arg.optionValue<int>( "rgb", 1 );
        const int b = arg.optionValue<int>( "rgb", 2 );
        color_map = kvs::RGBFormulae::Create( r, g, b, resolution );
    }
    else
    {
        if ( arg.hasOption( "PM3D" ) ) color_map = kvs::RGBFormulae::PM3D( resolution );
        else if ( arg.hasOption( "GreenRedViolet" ) ) color_map = kvs::RGBFormulae::GreenRedViolet( resolution );
        else if ( arg.hasOption( "Ocean" ) ) color_map = kvs::RGBFormulae::Ocean( resolution );
        else if ( arg.hasOption( "Hot" ) ) color_map = kvs::RGBFormulae::Hot( resolution );
        else if ( arg.hasOption( "ColorPrintable" ) ) color_map = kvs::RGBFormulae::ColorPrintable( resolution );
        else if ( arg.hasOption( "Rainbow" ) ) color_map = kvs::RGBFormulae::Rainbow( resolution );
        else if ( arg.hasOption( "AFMHot" ) ) color_map = kvs::RGBFormulae::AFMHot( resolution );
        else if ( arg.hasOption( "Jet" ) ) color_map = kvs::RGBFormulae::Jet( resolution );
        else if ( arg.hasOption( "Bone" ) ) color_map = kvs::RGBFormulae::Bone( resolution );
        else color_map.create();
    }

    kvs::OpacityMap opacity_map( resolution );
    if ( arg.hasOption( "opacity" ) )
    {
        int index = arg.optionValue<int>( "opacity" );
        bool negative = false;
        if ( index < 0 ) { negative = true; index *= -1; }

        index = kvs::Math::Clamp( index, 0, 42 );

        kvs::OpacityMap::Table table( resolution );
        float* opacity = table.pointer();
        for ( size_t i = 0; i < resolution; i++ )
        {
            const double x = static_cast<double>(i) / ( resolution - 1 );
            const double ox = negative ? 1.0 - x : x;
            const double o = kvs::Math::Clamp( ::f[index]( ox ), 0.0, 1.0 );
            *( opacity++ ) = static_cast<float>( o );
        }

        kvs::Real32 min_value = 0.0f;
        kvs::Real32 max_value = static_cast<kvs::Real32>( resolution - 1 );
        opacity_map = kvs::OpacityMap( table, min_value, max_value );
    }
    else
    {
        opacity_map.create();
    }

    kvs::TransferFunction tfunc;
    tfunc.setColorMap( color_map );
    tfunc.setOpacityMap( opacity_map );

    if ( tfunc.write( filename ) )
    {
        std::cout << "Write transfer function to " << filename << std::endl;
    }
    else
    {
        std::cout << "Cannot write transfer function to " << filename << std::endl;
    }

    return( 0 );
}
