#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

static void usage( const string& path )
{
    string program_name = path.substr( path.rfind( '\\' ) + 1 );
    cout << "Usage: " << program_name << " --in=name --out=name\n"
         << "   --in     name of the input file, e.g. input.txt\n"
         << "   --out    name of the output file, e.g. output.txt\n"
         << "Example:\n   "
         << program_name << " --in=input.txt --out=output.txt";
}

int main( int argc, char* argv[] )
{
    string in, out;
    if ( argc == 1 ) {
        in = "input.txt";
        out = "output.txt";
    } else if ( argc != 3 ) {
        usage( argv[0] );
        return 0;
    } else {
        for ( size_t i = 1; i <= 2; i++ ) {
            string arg = argv[i];
            size_t pos = arg.find( '=' );
            if ( pos == string::npos ) {
                usage( argv[0] );
                return 0;
            }
            string param = arg.substr( 0, pos );
            if ( param == "--in" )
                in = arg.substr( pos + 1 );
            else if ( param == "--out" )
                out = arg.substr( pos + 1 );
            else {
                usage( argv[0]     );
                return 0;
            }
        }
    }
    if ( in == out ) {
        cout << "Input and output files must be different.";
        return 0;
    }

    fstream file( in, ios_base::in );
    if ( !file ) {
        cout << "File " << in << " does not exist.";
        return 0;
    }

    size_t n;
    file >> n;
    typedef pair<size_t, size_t > Node; // (probability, huffman node index)
    typedef pair<size_t, size_t > HuffmanNode; // (left_child, right_child)
    vector<HuffmanNode> huffman_tree;
    priority_queue< Node, vector<Node>, greater<Node> > slice;
    vector<size_t> probability;
    size_t total_characters = 0;

    for ( size_t i = 0, p; i < n; i++ ) {
        file >> p;
        total_characters += p;
        probability.push_back( p );
        slice.push( make_pair( p, i ) );
        huffman_tree.push_back( make_pair( i, i ) );
    }
    file.close();

    file.open( out, ios_base::out );
    if ( !file ) {
        cout << "File " << out << " does not exist.";
        return 0;
    }

    while ( slice.size() >= 2 ) {
        Node left_child = slice.top(); slice.pop();
        Node right_child = slice.top(); slice.pop();

        huffman_tree.push_back(
            make_pair( left_child.second, right_child.second ) );

        size_t p = left_child.first + right_child.first;
        slice.push( make_pair( p, huffman_tree.size() - 1 ) );
    }

    vector<string> codes( huffman_tree.size(), "" );

    for ( size_t i = codes.size() - 1; i >= n; i-- ) {
        codes[ huffman_tree[i].first ] = codes[i] + "1";
        codes[ huffman_tree[i].second ] = codes[i] + "0";
    }

    if ( n == 1 )
        file << 0;
    else {
        for ( size_t i = 0; i < n; i++ )
            file << probability[i] << ": " << codes[i] << "\n";
    }

    float avg_code_length = 0;
    float max_entropy = 0;
    float entropy = 0;
    float redundance_coeff = 0;
    for ( size_t i = 0; i < n; i++ )
        avg_code_length += codes[i].size() * probability[i];
    avg_code_length /= total_characters;
    cout << "average code length: " << avg_code_length << "\n";
    max_entropy = avg_code_length * 1;
    cout << "max entropy: " << max_entropy << "\n";
    for ( size_t i = 0; i < n; i++ )
        entropy -= probability[i] *
            log2( probability[i] / float(total_characters) );
    entropy /= total_characters;
    cout << "entropy: " << entropy << "\n";
    redundance_coeff = (max_entropy - entropy) / max_entropy;
    cout << "redundance coefficient: " << redundance_coeff << "\n";

    file.close();
}
