using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace itp
{

    class Program
    {

        static Tuple<itp.Endpoint, itp.Endpoint> t01 = itp.Tunnel.Join();
        static Tuple<itp.Endpoint, itp.Endpoint> t02 = itp.Tunnel.Join();
        static Tuple<itp.Endpoint, itp.Endpoint> t13 = itp.Tunnel.Join();
        static Tuple<itp.Endpoint, itp.Endpoint> t14 = itp.Tunnel.Join();
        static Tuple<itp.Endpoint, itp.Endpoint> t45 = itp.Tunnel.Join();
        static Tuple<itp.Endpoint, itp.Endpoint> t06 = itp.Tunnel.Join();
        static Tuple<itp.Endpoint, itp.Endpoint> t67 = itp.Tunnel.Join();

        static ulong GetTime()
        {
            return (ulong)DateTimeOffset.Now.ToUnixTimeMilliseconds();
        }

        static void on_error(itp.Root root, byte address, itp.ErrorCode error)
        {
            Console.WriteLine("Node " + address + " throws error " + error);
        }

        static void on_connect(itp.Root root, byte address, ushort error)
        {
            if (error > 0)
            {
                Console.WriteLine("Failed to connect node " + address + ", error " + error + " - " + itp.Tools.GetErrorDescription(error));
            }
            else Console.WriteLine("Node with address " + address + " connected");
        }

        static itp.Endpoint on_create_1(string path, ref ushort error) {
            if (path == "3")
            {
                if (t13.Item1 == null)
                {
                    error = itp.Error.NULL_POINTER;
                    return null;
                }
                else {
                    error = itp.Error.NONE;
                    return t13.Item1;
                }
            }
            else if (path == "4")
            {
                if (t14.Item1 == null)
                {
                    error = itp.Error.NULL_POINTER;
                    return null;
                }
                else
                {
                    error = itp.Error.NONE;
                    return t14.Item1;
                }
            }
            else
            {
                error = itp.Error.WRONG_PATH;
                return null;
            }
        }

        static itp.Endpoint on_create_4(string path, ref ushort error)
        {
            if (path == "5")
            {
                if (t45.Item1 == null)
                {
                    error = itp.Error.NULL_POINTER;
                    return null;
                }
                else
                {
                    error = itp.Error.NONE;
                    return t45.Item1;
                }
            }
            else
            {
                error = itp.Error.WRONG_PATH;
                return null;
            }
        }


        static ushort on_request_0101(itp.Root root, itp.Frame frame)
        {
            ushort value = 0;
            frame.ReadUInt16(out value);
            Console.WriteLine("Node " + root.Address + " received value " + value + " from " + frame.From);
            value *= 2;
            itp.Frame response = new itp.Frame(0x0101);
            response.WriteUInt16(value);
            {
                ushort[] a1 = { 1, 2, 3 };
                uint[] a2 = { 4, 5, 6 };
                string s = "It Works!";
                response.WriteArrayUInt16(a1);
                response.WriteArrayUInt32(a2);
                response.WriteString(s);
            }
            root.PushResponse(frame, response, null);
            return itp.Error.NONE;
        }

        static ushort on_request_0102(itp.Root root, itp.Frame frame)
        {
            Console.WriteLine("Node " + root.Address + " received 0x" + frame.Command.ToString("X") + " from " + frame.From);
            return itp.Error.BAD_DATA;
        }

        static void on_response_0101(itp.Root root, ushort error, itp.Frame frame)
        {
            Console.WriteLine("Received result 0x" + error.ToString("X") + " from node " + frame.From);
            if (error == itp.Error.NONE)
            {
                ushort value = 0;
                frame.ReadUInt16(out value);
                Console.WriteLine("Received value: " + value);
                {
                    ushort[] a1 = { };
                    uint[] a2 = { };
                    string s = "";
                    frame.ReadArrayUInt16(ref a1);
                    frame.ReadArrayUInt32(ref a2);
                    frame.ReadString(out s);
                    Console.WriteLine("a1: [" + string.Join(", ", a1) + "]; a2: [" + string.Join(", ", a2) + "]; str: " + s);
                }
            }
        }

        static void on_response_0102(itp.Root root, ushort error, itp.Frame frame)
        {
            Console.WriteLine("Received result 0x" + error.ToString("X") + " from node " + frame.From);
        }

        static void on_receive_data(itp.Root root, byte[] data)
        {
            Console.WriteLine("Was received data: [" + string.Join(", ", data) + "]");
        }

        static void on_transmit_data(itp.Root root, ushort error)
        {
            Console.WriteLine("Data was sent with result 0x" + error.ToString("X"));
        }

        static void on_trace_route(itp.Root root, ushort error, itp.Frame frame)
        {
            Console.WriteLine("Trace route result: 0x" + error.ToString("X"));
            if (error == itp.Error.NONE)
            {
                byte[] data = { };
                frame.ReadArrayUInt8(ref data);
                Console.Write("Route: ");
                for (int i = 0; i < data.Length; ++i) {
                    if (i > 0) Console.Write(", ");
                    Console.Write(data[i]);
                }
                Console.WriteLine(" (" + data.Length + ")");
            }
        }

        static void on_connect_request(itp.Root root, ushort error)
        {
            Console.WriteLine("Connect request result: 0x" + error.ToString("X"));
        }

        static void Main(string[] args)
        {
            ulong time;
            Root n0 = new Root();
            Root n1 = new Root();
            Root n2 = new Root();
            Root n3 = new Root();
            Root n4 = new Root();
            Root n5 = new Root();
            Root n6 = new Root();
            Root n7 = new Root();
            n1.SetParent(t01.Item2, on_error);
            n2.SetParent(t02.Item2, on_error);
            n3.SetParent(t13.Item2, on_error);
            n4.SetParent(t14.Item2, on_error);
            n5.SetParent(t45.Item2, on_error);
            n6.SetParent(t06.Item2, on_error);
            n7.SetParent(t67.Item2, on_error);
            n1.SetOnCreate(on_create_1);
            n4.SetOnCreate(on_create_4);
            //
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                n6.Poll(time);
                n7.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            //
            Console.WriteLine("Initialization done.");
            //
            time = GetTime();
            byte a_n0 = 0xFF;
            n0.Address = a_n0;
            byte a_n1 = 0;
            n0.GetFreeAddress(ref a_n1);
            n0.ConnectLocalNode(time, t01.Item1, a_n1, on_connect);
            byte a_n2 = 0;
            n0.GetFreeAddress(ref a_n2);
            n0.ConnectLocalNode(time, t02.Item1, a_n2, on_connect);
            //
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            //
            Console.WriteLine("Stage 1 done.");
            //
            time = GetTime();
            byte a_n3 = 0;
            n0.GetFreeAddress(ref a_n3);
            n0.ConnectRemoteNode(time, a_n1, a_n3, "3", on_connect);
            byte a_n4 = 0;
            n0.GetFreeAddress(ref a_n4);
            n0.ConnectRemoteNode(time, a_n1, a_n4, "4", on_connect);
            //
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            //
            Console.WriteLine("Stage 2 done.");
            //
            time = GetTime();
            byte a_n5 = 0;
            n0.GetFreeAddress(ref a_n5);
            n0.ConnectRemoteNode(time, a_n4, a_n5, "5", on_connect);
            //
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 3 done.");
            //
            n5.RegisterHandler(a_n0, 0x0101, on_request_0101);
            {
                ushort value = 111;
                itp.Frame request = new itp.Frame(0x0101);
                request.WriteUInt16(value);
                n0.PushRequest(request, a_n5, on_response_0101);
            }
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 4 done.");
            //
            n5.TraceRoute(2, on_trace_route);
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 5 done.");
            //
            n1.RegisterHandler(a_n0, 0x0102, on_request_0102);
            {
                itp.Frame request = new itp.Frame(0x0102);
                n0.PushRequest(request, a_n1, on_response_0102);
            }
            for (int i = 0; i < 10; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 6 done.");
            //
            n0.SetConnectionListener(t06.Item1);
            n6.RequestConnect(time, on_connect_request);
            for (int i = 0; i < 30; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                n6.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 7 done.");
            //
            n6.SetConnectionListener(t67.Item1);
            n7.RequestConnect(time, on_connect_request);
            for (int i = 0; i < 30; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                n6.Poll(time);
                n7.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 8 done.");
            //
            n1.TraceRoute(7, on_trace_route);
            for (int i = 0; i < 30; i++)
            {
                time = GetTime();
                n0.Poll(time);
                n1.Poll(time);
                n2.Poll(time);
                n3.Poll(time);
                n4.Poll(time);
                n5.Poll(time);
                n6.Poll(time);
                n7.Poll(time);
                System.Threading.Thread.Sleep(3);
            }
            Console.WriteLine("Stage 9 done.");
            Console.ReadLine();
        }
    }
}
