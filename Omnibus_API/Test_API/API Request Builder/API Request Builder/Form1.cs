using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
//Includes
using System.Net.Http;
using System.Net.Http.Headers;
using System.Security.Permissions;
using System.Security.Policy;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading;
using System.Threading.Tasks;
using System.Web;
using System.Windows.Forms;
using static API_Request_Builder.Form1;
using static System.DateTimeOffset;
using static System.Net.WebRequestMethods;

namespace API_Request_Builder
{
    public partial class Form1 : Form
    {
        const int FRAME_LENGTH = 24;

        //Zero-side (sending to Pico)
        const string EPOCH_PREFIX = "TIME";
        const string STOP_VALIDITY_PREFIX = "SVAL";
        const string LINE_VALIDITY_PREFIX = "LVAL";
        const string LINE_NUMBER_PREFIX = "LNBR";
        const string LINE_IDENTIFIER_PREFIX = "LIDN";
        const string DIRECTIONS_PREFIX = "DIRE";
        const string STOP_NUMBER_PREFIX = "SNBR";
        const string STOP_IDENTIFIER_PREFIX = "SIDN";
        const string DONE_PREFIX = "DONE";
        const string TIME_REMAINING1_PREFIX = "REM1";
        const string TIME_REMAINING2_PREFIX = "REM2";

        //Pico-side (sent to Zero)
        const int CHECK_STOP = 0;
        const int CHECK_LINE = 1;
        const int GET_LINES = 2;
        const int ACK_LINE_QUANTITY = 3;
        const int SET_DIRECTION = 4;
        const int GET_DIRECTIONS = 5;
        const int GET_STOPS = 6;
        const int ACK_STOP_QUANTITY = 7;
        const int GET_TIMES = 8;
        const int RESET = 9;

        const bool NORMAL_PADDING = false;
        const bool STRING_PADDING = true;

        string UART_Buffer_In = "";
        string UART_Buffer_Out = "";

        const int MaxPassages = 3;
        const int NoBusSentinel = 9999;   // can't be a real minute value — marks an empty slot

        public class Stop
        {
            public string stopTransitlandID { get; set; }
            public string stopID { get; set; }
            public string stopName { get; set; }
            public List<Route> routesAtThisStop { get; set; } = new List<Route>();
            public List<Departure> stopDepartures { get; set; } = new List<Departure>();
            public DateTimeOffset departuresFetchedAt { get; set; }   // this stop's own freshness clock
            [JsonIgnore]
            public readonly object depLock = new object();   // guards stopDepartures; never serialized
        }

        public class Route
        {
            public string routeID { get; set; }
            public string routeName { get; set; }
            public string routeOnestopID { get; set; }
            public List<Stop> stopsOnThisRoute { get; set; } = new List<Stop>();
            public List<Direction> directionsOnThisRoute { get; set; } = new List<Direction>();
            public bool directionsLoaded { get; set; } = false;
        }

        public class Direction
        {
            public int directionId { get; set; }
            public string headsign { get; set; }
            public string representativeTripID { get; set; }   // integer id from trips list
            public int stopPatternId { get; set; }
            public List<Stop> stopsInThisDirection { get; set; } = new List<Stop>();
            public List<string> additionalTripIDs { get; set; }
            public bool stopsLoaded { get; set; } = false;
        }
        public class Departure
        {
            public DateTimeOffset time { get; set; }     // stored as UTC instant
            public string routeID { get; set; }
            public int directionId { get; set; } = -1;   // -1 = unknown, can't collide with 0/1
            public string headsign { get; set; }
            public string tripID { get; set; }
        }

        Dictionary<string, Stop> stopsDict = new Dictionary<string, Stop>();
        Dictionary<string, Route> routesDict = new Dictionary<string, Route>();

        HttpClient client = new HttpClient();

        string incoming_stop_or_line_str;
        string saved_stop_or_line_str;
        int saved_direction;
        bool waitTimesReady;
        bool refresh_tick = false;

        public struct RequestedWaitTimes
        {
            public string stopKey;
            public string routeKey;
        }
        Dictionary<int, RequestedWaitTimes> waitTimesDict = new Dictionary<int, RequestedWaitTimes>();
        int waitTimesIndex;

        public struct Frame
        {
            public string header;
            public int action;
        }

        public class FrameRouter
        {
            static Frame[] frame_headers = new Frame[]
            {
                new Frame { header = "(SCHK", action = CHECK_STOP},
                new Frame { header = "(LCHK", action = CHECK_LINE},
                new Frame { header = "(LGET",  action = GET_LINES},
                new Frame { header = "(LACK", action = ACK_LINE_QUANTITY},
                new Frame { header = "(DGET", action = GET_DIRECTIONS},
                new Frame { header = "(DSET",  action = SET_DIRECTION},
                new Frame { header = "(SGET",  action = GET_STOPS},
                new Frame { header = "(SACK", action = ACK_STOP_QUANTITY},
                new Frame { header = "(WAIT", action = GET_TIMES},
                new Frame { header = "(RESE", action = RESET}
            };

            public static int GetAction(string received)
            {
                for (int i = 0; i < frame_headers.Length; i++)
                {
                    if (received.StartsWith(frame_headers[i].header))
                        return frame_headers[i].action;
                }
                return -1; // unknown header
            }
            public static string GetStopLine(string received)
            {
                string numbers = string.Concat(received.Where(char.IsDigit));
                return numbers;
            }
        }
        /// <summary>
        /// API-related.
        /// </summary>

        string cacheFilePath = "rtc_cache.json";

        string apiKey = "QDpPOf8FwRlWDnyRYcNjnobDZm7ZzaNP";
        string dataQuery = "https://transit.land/api/v2/rest/routes?operator_onestop_id=o-f2m6-rseaudetransportdelacapitalertc";
        string departuresQuery1 = "https://transit.land/api/v2/rest/stops/";
        string departuresQuery2 = "/departures?";
        string resultsNumber = "&limit=1000";
        string includeStops = "&include_stops=true";
        string start_time = "start_time=00:00:00"; //pas de & après le "?"
        string end_time = "&end_time=23:59:59";
        string relative_date = "&relative_date=TODAY";
        string tripsQueryPrefix = "https://transit.land/api/v2/rest/routes/";
        string tripsQuerySuffix = "/trips?limit=100";
        string tripsRelativeDate = "&relative_date=WEDNESDAY";


        //UI
        bool firstSelection;
        string selectedRoute;
        string selectedStop;
        string[] nextThreeDepartures = new string[3];

        public Form1()
        {
            InitializeComponent();
            vInit();
            vClientSetup();
        }

        /// <summary>
        /// Init function.
        /// </summary>
        private void vInit()
        {
            textBoxKey.Text = apiKey;
            textBoxDataQuery.Text = dataQuery + resultsNumber + includeStops;
            firstSelection = true;
            labelStatus.Text = "Standby";
            labelStatusDepartures.Text = "Standby";

            //COM Port handling
            string[] listeDesPorts = SerialPort.GetPortNames();
            comboBoxCOM.Items.Clear();
            comboBoxCOM.Items.AddRange(listeDesPorts);
            try
            {
                comboBoxCOM.SelectedItem = listeDesPorts.Last();
            }
            catch
            {
                MessageBox.Show("Erreur d'obtention des ports série.");
            }
            set_Active(false);
        }

        /// <summary>
        /// HTTP client setup.
        /// </summary>
        private void vClientSetup()
        {
            client.DefaultRequestHeaders.Add("apikey", apiKey);
        }

        private void timerTimeUpdate_Tick(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen == true)
            {
                TimeSpan t = DateTime.UtcNow - new DateTime(1970, 1, 1);
                int secondsSinceEpoch = (int)t.TotalSeconds;
                handle_epoch(secondsSinceEpoch);
            }

            //COM port validation
            if (UART_Buffer_Out != "")
            {
                rtf_UART.Invoke((Action)(() =>
                {
                    rtf_UART.AppendText(UART_Buffer_Out + "\n");
                }));
                UART_Buffer_Out = "";
            }


        }
        /// <summary>
        /// 
        /// </summary>
        private void SaveCacheToDisk()
        {
            var cacheObject = new
            {
                savedAt = DateTimeOffset.UtcNow,
                routes = routesDict.Values.Select(r => new
                {
                    r.routeID,
                    r.routeName,
                    r.routeOnestopID,
                    r.directionsLoaded,
                    stops = r.stopsOnThisRoute.Select(s => s.stopID).ToList(),
                    directions = r.directionsOnThisRoute.Select(d => new
                    {
                        d.directionId,
                        d.headsign,
                        d.representativeTripID,
                        d.stopPatternId,
                        d.stopsLoaded,
                        stops = d.stopsInThisDirection.Select(s => s.stopID).ToList(),
                        additionalTripIDs = d.additionalTripIDs ?? new List<string>()
                    }).ToList()
                }).ToList(),
                stops = stopsDict.Values.Select(s => new
                {
                    s.stopID,
                    s.stopTransitlandID,
                    s.stopName,
                    routes = s.routesAtThisStop.Select(r => r.routeID).ToList(),
                    // NEW — its own freshness clock travels with the data
                    departuresFetchedAt = s.departuresFetchedAt,
                    departures = s.stopDepartures.Select(d => new
                    {
                        d.time,
                        d.routeID,
                        d.directionId,
                        d.headsign,
                        d.tripID
                    }).ToList()
                }).ToList()
            };

            string json = JsonSerializer.Serialize(cacheObject,
                new JsonSerializerOptions { WriteIndented = true });
            System.IO.File.WriteAllText(cacheFilePath, json);
            labelStatus.Text = "Cache saved to disk";
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        private bool TryLoadCacheFromDisk()
        {
            if (!System.IO.File.Exists(cacheFilePath))
                return false;

            try
            {
                string json = System.IO.File.ReadAllText(cacheFilePath);
                JsonDocument doc = JsonDocument.Parse(json);
                JsonElement root = doc.RootElement;

                // Check age
                DateTimeOffset savedAt = DateTimeOffset.Parse(
                    root.GetProperty("savedAt").GetString());

                // First pass: build all Stop objects (no route links yet)
                foreach (JsonElement s in root.GetProperty("stops").EnumerateArray())
                {
                    Stop stop = new Stop();
                    stop.stopID = s.GetProperty("stopID").GetString();
                    stop.stopTransitlandID = s.GetProperty("stopTransitlandID").GetString();
                    stop.stopName = s.GetProperty("stopName").GetString();

                    if (s.TryGetProperty("departuresFetchedAt", out var fa)
                        && fa.ValueKind == JsonValueKind.String)
                        stop.departuresFetchedAt = DateTimeOffset.Parse(
                            fa.GetString(), CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind);

                    if (s.TryGetProperty("departures", out var deps) && deps.ValueKind == JsonValueKind.Array)
                    {
                        DateTimeOffset now = DateTimeOffset.UtcNow;
                        foreach (JsonElement d in deps.EnumerateArray())
                        {
                            DateTimeOffset t = DateTimeOffset.Parse(
                                d.GetProperty("time").GetString(),
                                CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind);
                            if (t < now) continue;   // drop stale past departures at load time

                            stop.stopDepartures.Add(new Departure
                            {
                                time = t,
                                routeID = d.GetProperty("routeID").GetString(),
                                directionId = d.TryGetProperty("directionId", out var di) ? di.GetInt32() : -1,
                                headsign = d.TryGetProperty("headsign", out var hs)
                                                  && hs.ValueKind == JsonValueKind.String ? hs.GetString() : null,
                                tripID = d.TryGetProperty("tripID", out var ti)
                                                  && ti.ValueKind == JsonValueKind.String ? ti.GetString() : null,
                            });
                        }
                    }

                    stopsDict[stop.stopID] = stop;
                }

                // Second pass: build Route and Direction objects (no cross-links yet)
                foreach (JsonElement r in root.GetProperty("routes").EnumerateArray())
                {
                    Route route = new Route();
                    route.routeID = r.GetProperty("routeID").GetString();
                    route.routeName = r.GetProperty("routeName").GetString();
                    route.routeOnestopID = r.GetProperty("routeOnestopID").GetString();
                    route.directionsLoaded = r.GetProperty("directionsLoaded").GetBoolean();

                    foreach (JsonElement d in r.GetProperty("directions").EnumerateArray())
                    {
                        Direction direction = new Direction();
                        direction.directionId = d.GetProperty("directionId").GetInt32();
                        direction.headsign = d.GetProperty("headsign").GetString();
                        direction.representativeTripID = d.GetProperty("representativeTripID").GetString();
                        direction.stopPatternId = d.GetProperty("stopPatternId").GetInt32();
                        direction.stopsLoaded = d.GetProperty("stopsLoaded").GetBoolean();

                        foreach (JsonElement stopID in d.GetProperty("stops").EnumerateArray())
                        {
                            if (stopsDict.TryGetValue(stopID.GetString(), out Stop knownStop))
                                direction.stopsInThisDirection.Add(knownStop);
                        }

                        var extraIDs = d.GetProperty("additionalTripIDs");
                        if (extraIDs.GetArrayLength() > 0)
                        {
                            direction.additionalTripIDs = new List<string>();
                            foreach (JsonElement id in extraIDs.EnumerateArray())
                                direction.additionalTripIDs.Add(id.GetString());
                        }

                        route.directionsOnThisRoute.Add(direction);
                    }

                    routesDict[route.routeID] = route;
                }

                // Third pass: resolve cross-links now that both dicts are populated
                foreach (JsonElement r in root.GetProperty("routes").EnumerateArray())
                {
                    string routeID = r.GetProperty("routeID").GetString();
                    Route route = routesDict[routeID];

                    foreach (JsonElement stopID in r.GetProperty("stops").EnumerateArray())
                    {
                        if (stopsDict.TryGetValue(stopID.GetString(), out Stop stop))
                            route.stopsOnThisRoute.Add(stop);
                    }
                }

                foreach (JsonElement s in root.GetProperty("stops").EnumerateArray())
                {
                    string stopID = s.GetProperty("stopID").GetString();
                    Stop stop = stopsDict[stopID];

                    foreach (JsonElement routeID in s.GetProperty("routes").EnumerateArray())
                    {
                        if (routesDict.TryGetValue(routeID.GetString(), out Route route))
                            stop.routesAtThisStop.Add(route);
                    }
                }
                populateRoutesComboBox();
                populateStopsComboBox();
                labelStatus.Text = "Cache loaded from disk";
                return true;
            }
            catch (Exception ex)
            {
                // Corrupt or incompatible cache — fall through to API
                labelStatus.Text = $"Cache load failed ({ex.Message}), reloading from API";
                stopsDict.Clear();
                routesDict.Clear();
                return false;
            }
        }


        private async void buttonDataQuery_Click(object sender, EventArgs e)
        {
            string nextURL = textBoxDataQuery.Text;
            labelStatus.Text = "Request Ongoing";
            while (nextURL != null)
            {

                //Attendre une réponse
                var response = await client.GetAsync(nextURL);
                response.EnsureSuccessStatusCode();
                labelStatus.Text = "API Reply";

                //Faire un objet JSON avec une racine
                var json = await response.Content.ReadAsStringAsync();
                JsonDocument doc = JsonDocument.Parse(json);
                JsonElement root = doc.RootElement;
                labelStatus.Text = "API Data Inboud";

                try
                {
                    //Créer les éléments "route/stopArray" en cherchant à partir de la racine
                    JsonElement routeArray = root.GetProperty("routes");

                    labelStatus.Text = "Parsing...";
                    //Trouver tous les codes d'arrêts liés aux routes 
                    foreach (JsonElement route in routeArray.EnumerateArray())
                    {
                        Route routeObject = new Route();
                        routeObject.routeOnestopID = route.GetProperty("onestop_id").GetString();
                        routeObject.routeName = route.GetProperty("route_short_name").GetString();
                        routeObject.routeID = route.GetProperty("route_id").GetString();

                        //Ajout au dictionnaire d'une entrée qui portera le nom du ID, et qui contiendra
                        //un objet "Route".
                        routesDict[routeObject.routeID] = routeObject;

                        //Découpage du champ suivant en récupérant route_stops et en le transformant en
                        //JsonElement
                        route.TryGetProperty("route_stops", out JsonElement stopArray);

                        foreach (JsonElement stop in stopArray.EnumerateArray())
                        {
                            Stop stopObject = new Stop();
                            Stop stopCanonical = new Stop();

                            stopObject.stopTransitlandID = stop.GetProperty("stop").GetProperty("id").GetInt64().ToString();
                            stopObject.stopID = stop.GetProperty("stop").GetProperty("stop_id").GetString();
                            stopObject.stopName = stop.GetProperty("stop").GetProperty("stop_name").GetString();

                            //Si le dictionnaire des objets d'arrêts n'a pas de clé pour un arrêt,
                            //on crée une entrée appellée "stopID" et on lui associe le stopObject,
                            //auquel on aura aussi ajouté le routeObject.
                            if (!stopsDict.ContainsKey(stopObject.stopID))
                            {
                                stopsDict[stopObject.stopID] = stopObject;
                                stopCanonical = stopObject;
                            }

                            //Sinon, on récupère la clé liée à cet arrêt.
                            else
                            {
                                stopCanonical = stopsDict[stopObject.stopID];
                            }

                            stopCanonical.routesAtThisStop.Add(routeObject);
                            routeObject.stopsOnThisRoute.Add(stopCanonical);
                        }
                    }
                }
                catch
                {
                    labelStatus.Text = "Parse unsuccessful";
                    break;
                }
                //Si le header JSON contient une propriété "meta", vérifier si son
                //champ n'est pas vide, car ça indique qu'on peut continuer à obtenir
                //des données.
                try
                {
                    JsonElement meta = root.GetProperty("meta");
                    if (meta.TryGetProperty("next", out JsonElement nextElement)
                    && nextElement.ValueKind != JsonValueKind.Null)
                    {
                        nextURL = nextElement.GetString();
                    }
                    else
                    {
                        nextURL = null;
                    }
                }
                catch
                {
                    nextURL = null;
                }
                populateRoutesComboBox();
                populateStopsComboBox();
                labelStatus.Text = "Request Completed";
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void buttonDeparturesQuery_Click(object sender, EventArgs e)
        {
            await handleDeparturesQuery(textBoxDeparturesQuery.Text, textBoxSelectedStop.Text, textBoxSelectedRoute.Text);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="nextURL"></param>
        /// <param name="stop"></param>
        /// <param name="line"></param>
        /// <returns></returns>
        private async Task handleDeparturesQuery(string nextURL, string stop, string line)
        {
            rtfBox_json.Invoke((Action)(() =>
            {
                 rtfBox_json.Clear();
            }));
            labelStatusDepartures.Invoke((Action)(() =>
            {
                labelStatusDepartures.Text = "Request Ongoing";
            }));


            // Hoisted: resolve the zone once, accumulate across all pages.
            TimeZoneInfo localZone = TimeZoneInfo.FindSystemTimeZoneById("Eastern Standard Time");
            var collected = new List<Departure>();
            string targetRoute = "1-" + line;
            string stopKey = "1-" + stop;

            while (nextURL != null)
            {
                var response = await client.GetAsync(nextURL);
                response.EnsureSuccessStatusCode();
                labelStatusDepartures.Invoke((Action)(() =>
                {
                    labelStatusDepartures.Text = "API Reply";
                }));
                var json = await response.Content.ReadAsStringAsync();
                JsonDocument doc = JsonDocument.Parse(json);   // dispose pooled buffers each page
                JsonElement root = doc.RootElement;
                JsonElement departureArray = root.GetProperty("stops").EnumerateArray().First();
                labelStatusDepartures.Invoke((Action)(() =>
                {
                    labelStatusDepartures.Text = "Parsing...";
                }));
                

                foreach (JsonElement departures in departureArray.GetProperty("departures").EnumerateArray())
                {
                    JsonElement trip = departures.GetProperty("trip");
                    string routeID = trip.GetProperty("route").GetProperty("route_id").GetString();
                    if (routeID != targetRoute) continue;

                    JsonElement depObj = departures.GetProperty("departure");

                    // Realtime estimate when present, else the schedule.
                    string instant =
                        depObj.TryGetProperty("estimated_utc", out var est)
                            && est.ValueKind == JsonValueKind.String
                            ? est.GetString()
                            : depObj.GetProperty("scheduled_utc").GetString();

                    if (string.IsNullOrEmpty(instant)) continue;   // guard: no usable time

                    DateTimeOffset utc = DateTimeOffset.Parse(
                        instant, CultureInfo.InvariantCulture, DateTimeStyles.AssumeUniversal);

                    collected.Add(new Departure
                    {
                        time = utc,
                        routeID = routeID,
                        tripID = trip.TryGetProperty("trip_id", out var ti) ? ti.GetString() : null,
                        directionId = trip.TryGetProperty("direction_id", out var di)
                                          && di.ValueKind == JsonValueKind.Number ? di.GetInt32() : -1,
                        headsign = trip.TryGetProperty("trip_headsign", out var hs)
                                          && hs.ValueKind == JsonValueKind.String ? hs.GetString() : null,
                    });

                    var local = TimeZoneInfo.ConvertTime(utc, localZone);
                    rtfBoxAllDepartures.Invoke((Action)(() =>
                    {
                        rtfBoxAllDepartures.Text += local.ToString("HH:mm") + ' ';
                    }));
                    
                }

                try   // pagination, unchanged
                {
                    JsonElement meta = root.GetProperty("meta");
                    if (meta.TryGetProperty("next", out JsonElement nextElement)
                        && nextElement.ValueKind != JsonValueKind.Null)
                        nextURL = nextElement.GetString();
                    else
                        nextURL = null;
                }
                catch { nextURL = null; }
            }

            // Commit once, after all pages. Per-line replace = no duplicates, no cross-line wipeout.
            if (stopsDict.TryGetValue(stopKey, out var stopObj))
            {
                lock (stopObj.depLock)                                  // no await inside this block — required
                {
                    stopObj.stopDepartures.RemoveAll(d => d.routeID == targetRoute);
                    stopObj.stopDepartures.AddRange(collected);
                    stopObj.departuresFetchedAt = DateTimeOffset.UtcNow;
                }
                waitTimesReady = true;
                labelStatusDepartures.Invoke((Action)(() =>
                {
                    labelStatusDepartures.Text = "Request Completed";
                }));
                
            }
            else
            {
                labelStatusDepartures.Invoke((Action)(() =>
                {
                    labelStatusDepartures.Text = "Unknown stop: " + stopKey;   // don't index a missing key
                }));
                
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comboBoxAvailableRoutes_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxAvailableRoutes.SelectedItem == null) return;
            handle_AvailableRoutes(comboBoxAvailableRoutes.SelectedItem.ToString());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="currentRoute"></param>
        private async void handle_AvailableRoutes(string currentRoute)
        {
            // Existing behavior: filter stops to all stops on the route (fallback)
            filterStopsComboBox(currentRoute);

            // New behavior: only if user is starting from a route (firstSelection
            // was true coming into filterStopsComboBox), populate directions.
            // filterStopsComboBox already flipped firstSelection -- we need a snapshot.
            // Simplest: detect via routesDict having an onestop_id.
            if (!routesDict.ContainsKey(currentRoute)) return;
            Route route = routesDict[currentRoute];
            if (string.IsNullOrEmpty(route.routeOnestopID)) return;

            await LoadDirectionsForRouteAsync(route);
            populateDirectionsComboBox(route);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void comboBoxAvailableStops_SelectedIndexChanged(object sender, EventArgs e)
        {
            string currentStop = comboBoxAvailableStops.SelectedItem.ToString();

            filterRoutesComboBox(currentStop);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonResetSelection_Click(object sender, EventArgs e)
        {
            firstSelection = true;
            comboBoxAvailableRoutes.Items.Clear();
            comboBoxAvailableRoutes.Text = "";
            comboBoxAvailableStops.Items.Clear();
            comboBoxAvailableStops.Text = "";
            comboBoxAvailableDirections.Items.Clear();
            comboBoxAvailableDirections.Text = "";
            textBoxSelectedRoute.Text = "";
            textBoxSelectedStop.Text = "";
            rtfBoxAllDepartures.Clear();
            populateRoutesComboBox();
            populateStopsComboBox();
        }

        /// <summary>
        /// 
        /// </summary>
        private void populateStopsComboBox()
        {
            List<string> orderedStops = new List<string>();

            foreach (KeyValuePair<string, Stop> kvp in stopsDict)
            {
                string stopNameAndID = (kvp.Value.stopID + " " + kvp.Value.stopName);
                orderedStops.Add(stopNameAndID);
            }
            orderedStops.Sort();
            comboBoxAvailableStops.Items.AddRange(orderedStops.ToArray());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonConfirmSelection_Click(object sender, EventArgs e)
        {
            if ((comboBoxAvailableRoutes.Text != "") && (comboBoxAvailableStops.Text != ""))
            {
                string stop = comboBoxAvailableStops.SelectedItem.ToString();
                string line = comboBoxAvailableRoutes.SelectedItem.ToString();
                handle_API_URL(stop, line);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="stop"></param>
        /// <param name="line"></param>
        private async void handle_API_URL(string stop, string line)
        {
            string stopID = stop.Split(' ')[0];
            string desiredTransitLandID = stopsDict["1-" + stop].stopTransitlandID;

            string URL = departuresQuery1 +
                desiredTransitLandID +
                departuresQuery2 +
                start_time +
                end_time +
                resultsNumber +
                relative_date;

            textBoxDeparturesQuery.Invoke((Action)(() =>
            {
                textBoxDeparturesQuery.Text = URL;
            }));

            textBoxSelectedRoute.Invoke((Action)(() =>
            {
                textBoxSelectedRoute.Text = "1-" + line;
            }));

            textBoxSelectedStop.Invoke((Action)(() =>
            {
                textBoxSelectedStop.Text = stopID;
            }));
            
            await handleDeparturesQuery(URL, stop, line);
        }

        /// <summary>
        /// 
        /// </summary>
        private void populateRoutesComboBox()
        {
            comboBoxAvailableRoutes.Items.Clear(); //duplicate protection
            foreach (KeyValuePair<string, Route> kvp in routesDict)
            {
                comboBoxAvailableRoutes.Items.Add(kvp.Key);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="selectedRoute"></param>
        private void filterStopsComboBox(string selectedRoute)
        {
            List<string> orderedStops = new List<string>();

            if (firstSelection == true)
            {
                comboBoxAvailableStops.Items.Clear();
                foreach (Stop stop in routesDict[selectedRoute].stopsOnThisRoute)
                {
                    string stopNameAndID = (stop.stopID + " " + stop.stopName);
                    orderedStops.Add(stopNameAndID);
                }
                comboBoxAvailableStops.Items.AddRange(orderedStops.ToArray());
                firstSelection = false;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="selectedStop"></param>
        private void filterRoutesComboBox(string selectedStop)
        {
            string stopID = selectedStop.Split(' ')[0];

            if (firstSelection == true)
            {
                comboBoxAvailableRoutes.Items.Clear();
                foreach (Route route in stopsDict[stopID].routesAtThisStop)
                {
                    comboBoxAvailableRoutes.Items.Add(route.routeName);
                }
            }
            firstSelection = false;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="route"></param>
        /// <returns></returns>
        private async Task LoadDirectionsForRouteAsync(Route route)
        {
            if (route.directionsLoaded) return;

            string nextURL = tripsQueryPrefix + route.routeOnestopID
                            + tripsQuerySuffix + tripsRelativeDate;

            // Dedupe by (direction_id, headsign) — same destination = same UI entry.
            // The Dictionary value lets us look up the existing Direction so we can
            // append extra trip IDs to it for the union-of-stops merge later.
            var seenDirections = new Dictionary<string, Direction>();
            var patternsSeen = new Dictionary<string, HashSet<int>>();

            labelStatus.Invoke((Action)(() =>
            {
                labelStatus.Text = "Loading directions...";
            }));
            
            int pageCount = 0;
            while (nextURL != null && pageCount++ < 200)
            {
                var response = await client.GetAsync(nextURL);
                response.EnsureSuccessStatusCode();

                var json = await response.Content.ReadAsStringAsync();
                JsonDocument doc = JsonDocument.Parse(json);
                JsonElement root = doc.RootElement;

                if (!root.TryGetProperty("trips", out JsonElement tripArray))
                {
                    break;
                }

                foreach (JsonElement trip in tripArray.EnumerateArray())
                {
                    // ---- Extract fields up front, since we need them for the key ----

                    // stop_pattern_id — still useful for the canonical Direction and
                    // (importantly) for skipping trips whose pattern we already have.
                    if (!trip.TryGetProperty("stop_pattern_id", out JsonElement patternEl)
                        || patternEl.ValueKind != JsonValueKind.Number)
                    {
                        continue;
                    }
                    int patternId = patternEl.GetInt32();

                    // direction_id, defensively as before.
                    int dirId =
                        trip.TryGetProperty("direction_id", out JsonElement dirEl)
                        && dirEl.ValueKind == JsonValueKind.Number
                        ? dirEl.GetInt32() : -1;

                    // Headsign, defensively as before.
                    string headsign =
                        trip.TryGetProperty("trip_headsign", out JsonElement hsEl)
                        && hsEl.ValueKind == JsonValueKind.String
                        ? hsEl.GetString()
                        : $"Direction {dirId}";

                    // Integer id for the by-ID trips endpoint.
                    string tripIntID = trip.GetProperty("id").GetInt64().ToString();

                    // ---- Decide: new Direction, or merge into an existing one? ----

                    // The '|' is just a separator that can't appear in a direction_id
                    // (it's a small int). If you ever worry about headsigns containing
                    // '|' literally, swap for a tuple key or any unambiguous delimiter.
                    string key = dirId + "|" + headsign;


                    if (seenDirections.TryGetValue(key, out Direction existing))
                    {
                        // Keep exactly one trip per distinct stop_pattern_id.
                        if (patternsSeen[key].Add(patternId))
                        {
                            if (existing.additionalTripIDs == null)
                                existing.additionalTripIDs = new List<string>();
                            existing.additionalTripIDs.Add(tripIntID);
                        }
                        continue;
                    }

                    // First time we've seen this (direction_id, headsign) combo.
                    Direction direction = new Direction();
                    direction.directionId = dirId;
                    direction.headsign = headsign;
                    direction.stopPatternId = patternId;          // canonical pattern
                    direction.representativeTripID = tripIntID;   // canonical trip

                    route.directionsOnThisRoute.Add(direction);
                    seenDirections[key] = direction;
                    patternsSeen[key] = new HashSet<int> { patternId };
                }

                // Pagination
                if (root.TryGetProperty("meta", out JsonElement meta)
                    && meta.TryGetProperty("next", out JsonElement nextEl)
                    && nextEl.ValueKind == JsonValueKind.String)
                {
                    nextURL = nextEl.GetString();
                }
                else
                {
                    nextURL = null;
                }
            }
            route.directionsOnThisRoute.Sort((a, b) => a.directionId.CompareTo(b.directionId));
            route.directionsLoaded = true;
            labelStatus.Invoke((Action)(() =>
            {
                labelStatus.Text = "Directions loaded";
            }));
            await LoadStopsForDirectionAsync(route, route.directionsOnThisRoute.First());
            await LoadStopsForDirectionAsync(route, route.directionsOnThisRoute.Last());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="route"></param>
        /// <param name="direction"></param>
        /// <returns></returns>
        private async Task LoadStopsForDirectionAsync(Route route, Direction direction)
        {
            if (direction.stopsLoaded) return;

            // --- Step 1: Load the canonical (representative) trip's stops ---

            string url = tripsQueryPrefix + route.routeOnestopID
                         + "/trips/" + direction.representativeTripID;

            labelStatus.Invoke((Action)(() =>
            {
                labelStatus.Text = "Loading stops for direction...";
            }));


            var response = await client.GetAsync(url);
            response.EnsureSuccessStatusCode();

            var json = await response.Content.ReadAsStringAsync();
            JsonDocument doc = JsonDocument.Parse(json);
            JsonElement root = doc.RootElement;

            JsonElement tripsArray = root.GetProperty("trips");
            JsonElement trip = tripsArray.EnumerateArray().First();

            if (!trip.TryGetProperty("stop_times", out JsonElement stopTimes))
            {
                labelStatus.Invoke((Action)(() =>
                {
                    labelStatus.Text = "No stop_times in trip response";
                }));
                return;
            }

            // Track stops we've already added so we don't duplicate during the merge.
            // Built up once here, then reused in the variant loop below.
            var alreadyKnown = new HashSet<string>();

            foreach (JsonElement st in stopTimes.EnumerateArray())
            {
                JsonElement stopEl = st.GetProperty("stop");
                string gtfsStopID = stopEl.GetProperty("stop_id").GetString();

                if (stopsDict.TryGetValue(gtfsStopID, out Stop knownStop))
                {
                    direction.stopsInThisDirection.Add(knownStop);
                    alreadyKnown.Add(gtfsStopID);
                }
            }

            // --- Step 2: Merge in stops from any variant trips ---

            if (direction.additionalTripIDs != null)
            {
                foreach (string extraTripID in direction.additionalTripIDs)
                {
                    string variantUrl = tripsQueryPrefix + route.routeOnestopID
                                        + "/trips/" + extraTripID;

                    var variantResponse = await client.GetAsync(variantUrl);
                    variantResponse.EnsureSuccessStatusCode();

                    var variantJson = await variantResponse.Content.ReadAsStringAsync();
                    JsonDocument variantDoc = JsonDocument.Parse(variantJson);
                    JsonElement variantRoot = variantDoc.RootElement;

                    JsonElement variantTripsArray = variantRoot.GetProperty("trips");
                    JsonElement variantTrip = variantTripsArray.EnumerateArray().First();

                    if (!variantTrip.TryGetProperty("stop_times", out JsonElement variantStopTimes))
                    {
                        continue;   // skip silently, don't abort the whole merge
                    }

                    foreach (JsonElement st in variantStopTimes.EnumerateArray())
                    {
                        string gtfsStopID = st.GetProperty("stop").GetProperty("stop_id").GetString();

                        if (alreadyKnown.Contains(gtfsStopID)) continue;

                        if (stopsDict.TryGetValue(gtfsStopID, out Stop knownStop))
                        {
                            direction.stopsInThisDirection.Add(knownStop);
                            alreadyKnown.Add(gtfsStopID);
                        }
                    }
                }
            }

            direction.stopsLoaded = true;
            labelStatus.Invoke((Action)(() =>
            {
                labelStatus.Text = "Stops loaded";
            }));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void comboBoxAvailableDirections_SelectedIndexChanged(object sender, EventArgs e)
        {
            comboBoxAvailableDirections.Enabled = false;
            if (comboBoxAvailableDirections.SelectedItem == null) return;
            if (comboBoxAvailableRoutes.SelectedItem == null) return;

            string currentRoute = comboBoxAvailableRoutes.SelectedItem.ToString();
            if (!routesDict.ContainsKey(currentRoute)) return;
            Route route = routesDict[currentRoute];

            // The combo holds the Direction object directly so we can find it without
            // string parsing. See populateDirectionsComboBox below.
            Direction direction = comboBoxAvailableDirections.SelectedItem as Direction;
            if (direction == null) return;

            await LoadStopsForDirectionAsync(route, direction);
            filterStopsByDirection(direction);
            comboBoxAvailableDirections.Enabled = true;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="route"></param>
        private void populateDirectionsComboBox(Route route)
        {
            comboBoxAvailableDirections.Items.Clear();
            comboBoxAvailableDirections.DisplayMember = "headsign";  // shows Direction.headsign
            foreach (Direction d in route.directionsOnThisRoute)
            {
                comboBoxAvailableDirections.Items.Add(d);
            }
            // If only one direction exists (loop route, one-way), auto-select it.
            if (route.directionsOnThisRoute.Count == 1)
            {
                comboBoxAvailableDirections.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="direction"></param>
        private void filterStopsByDirection(Direction direction)
        {
            comboBoxAvailableStops.Items.Clear();
            List<string> orderedStops = new List<string>();
            foreach (Stop stop in direction.stopsInThisDirection)
            {
                orderedStops.Add(stop.stopID + " " + stop.stopName);
            }
            // Don't .Sort() here — direction stop order is meaningful (sequence
            // along the route). Sorting alphabetically destroys that information.
            comboBoxAvailableStops.Items.AddRange(orderedStops.ToArray());
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_Connect_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.PortName = (string)comboBoxCOM.SelectedItem;
                    serialPort1.BaudRate = 115200;
                    serialPort1.Encoding = Encoding.GetEncoding("iso-8859-1");
                    serialPort1.Open();
                    serialPort1.DiscardInBuffer();
                    button_Connect.Text = "Déconnecter";
                    set_Active(true);
                }
                catch
                {
                    MessageBox.Show("Problème lors de la connexion.",
                    "Erreur de connexion",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                }
            }
            else
            {
                try
                {
                    serialPort1.Close();
                    button_Connect.Text = "Connecter";
                    rtf_UART.Clear();
                    set_Active(false);
                }
                catch
                {
                    MessageBox.Show("Problème lors de la déconnexion.",
                    "Erreur de connexion",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                }
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="active_val"></param>
        private void set_Active(bool active_val)
        {
            button_Epoch.Enabled = active_val;
            button_LineGood.Enabled = active_val;
            button_LineBad.Enabled = active_val;
            button_StopGood.Enabled = active_val;
            button_StopBad.Enabled = active_val;
            button_NbrLines.Enabled = active_val;
            button_LineID.Enabled = active_val;
            button_Directions.Enabled = active_val;
            button_NbrStops.Enabled = active_val;
            button_StopID.Enabled = active_val;
            button_TimeRemaining.Enabled = active_val;
            button_DoneSending.Enabled = active_val;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="message"></param>
        /// <param name="length"></param>
        /// <param name="output"></param>
        /// <param name="string_padding"></param>
        private void send_frame(string message, int length, bool output, bool string_padding)
        {
            if (message.Length > 24)
            {
                message = message.Substring(0, 23);
                message += ")";
            }
            else if (string_padding)
            {
                for (int i = 0; i < (FRAME_LENGTH - length); i++)
                {
                    message += " ";
                }
            }
            else
            {
                for (int i = 0; i < (FRAME_LENGTH - length); i++)
                {
                    message += "+";
                }
            }
            byte[] txBuffer = Encoding.GetEncoding("iso-8859-1").GetBytes(message);
            serialPort1.Write(txBuffer, 0, txBuffer.Length);
            if (output)
            {
                rtf_UART.Invoke((Action)(() =>
                {
                    rtf_UART.AppendText(message);
                    rtf_UART.AppendText("\n");
                }));
                UART_Buffer_Out = "";
            }

            
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_Epoch_Click(object sender, EventArgs e)
        {
            TimeSpan t = DateTime.UtcNow - new DateTime(1970, 1, 1);
            int secondsSinceEpoch = (int)t.TotalSeconds;
            handle_epoch(secondsSinceEpoch);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="epoch_val"></param>
        private void handle_epoch(long epoch_val)
        {
            string message = "(" + EPOCH_PREFIX + "_" + epoch_val.ToString() + ")";
            int length = message.Length;
            send_frame(message, length, false, NORMAL_PADDING);
        }

        /// <summary>
        /// By stop!
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_StopGood_Click(object sender, EventArgs e)
        {
            handle_StopValidity("1591", "G");
        }

        private void button_StopBad_Click(object sender, EventArgs e)
        {
            handle_StopValidity("6666", "B");
        }

        private void handle_StopValidity(string stop, string good_or_bad)
        {
            string message = "(" + STOP_VALIDITY_PREFIX + "_" + stop + good_or_bad + ")";
            int length = message.Length;
            send_frame(message, length, true, NORMAL_PADDING);
        }

        private void button_NbrLines_Click(object sender, EventArgs e)
        {
            handle_NbrLines("1138");
        }

        private void handle_NbrLines(string stop)
        {
            saved_stop_or_line_str = stop;
            string message = "(" + LINE_NUMBER_PREFIX + "_" + (stopsDict[("1-" + stop)].routesAtThisStop.Count).ToString() + ")";
            int length = message.Length;
            send_frame(message, length, true, NORMAL_PADDING);
        }

        private void button_LineID_Click(object sender, EventArgs e)
        {
            saved_stop_or_line_str = "8002";
            handle_LineID();
        }

        private void handle_LineID()
        {
            foreach (Route route in stopsDict[("1-" + saved_stop_or_line_str)].routesAtThisStop)
            {
                string message = "(" + LINE_IDENTIFIER_PREFIX + "_" + route.routeName + ")";
                int length = message.Length;
                send_frame(message, length, true, NORMAL_PADDING);
                Thread.Sleep(10);
            }
            saved_stop_or_line_str = "";
        }

        /// <summary>
        /// By line!
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_LineGood_Click(object sender, EventArgs e)
        {
            handle_LineValidity("801", "G");
        }

        private void button_LineBad_Click(object sender, EventArgs e)
        {
            handle_LineValidity("100", "B");
        }

        private void handle_LineValidity(string line, string good_or_bad)
        {
            string message = "(" + LINE_VALIDITY_PREFIX + "_" + line + good_or_bad + ")";
            int length = message.Length;
            send_frame(message, length, true, NORMAL_PADDING);
        }

        private void button_Directions_Click(object sender, EventArgs e)
        {

        }

        private async void handle_directions(string line)
        {
            await LoadDirectionsForRouteAsync(routesDict["1-" + line]);
            saved_stop_or_line_str = line;
            foreach (Direction direction in routesDict["1-" + line].directionsOnThisRoute)
            {
                string message = "(" + DIRECTIONS_PREFIX + "_" + direction.headsign + ")";
                int length = message.Length;
                send_frame(message, length, true, NORMAL_PADDING);
                Thread.Sleep(10);
            }
            if (routesDict["1-" + line].directionsOnThisRoute.Count == 1)
            {
                string message = "(" + DIRECTIONS_PREFIX + "_" + "Pas de 2e direct." + ")";
                int length = message.Length;
                send_frame(message, length, true, NORMAL_PADDING);
            }
        }
        private void button_NbrStops_Click(object sender, EventArgs e)
        {

        }

        private void handle_NbrStops(string stops_direction)
        {
            string stop = stops_direction.Substring(0, 3);
            string direction_index = stops_direction.Substring(3, 1);
            saved_direction = int.Parse(direction_index);
            int choice_int = int.Parse(direction_index);

            var direction = routesDict["1-" + saved_stop_or_line_str].directionsOnThisRoute[choice_int];

            string message = "(" + STOP_NUMBER_PREFIX + "_" + (direction.stopsInThisDirection.Count).ToString() + ")";
            int length = message.Length;
            send_frame(message, length, true, NORMAL_PADDING);
        }

        private void button_StopID_Click(object sender, EventArgs e)
        {

        }

        private void handle_StopID()
        {
            var direction = routesDict["1-" + saved_stop_or_line_str].directionsOnThisRoute[saved_direction];
            foreach (Stop stop in direction.stopsInThisDirection)
            {
                string message = "(" + STOP_IDENTIFIER_PREFIX + "_" + stop.stopID.Substring(2) + "_" + stop.stopName; //No closing parenthesis here for string display
                int length = message.Length;
                send_frame(message, length, true, STRING_PADDING);
                Thread.Sleep(10);
            }
        }

        private void button_TimeRemaining_Click(object sender, EventArgs e)
        {
            handle_TimeRemaining("1211", "801");
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="stop"></param>
        /// <param name="line"></param>
        private void handle_TimeRemaining(string stop, string line)
        {
            waitTimesReady = false;
            handle_API_URL(stop, line);

            string stopKey = "1-" + stop;
            string routeKey = "1-" + line;

            RequestedWaitTimes savedTime = new RequestedWaitTimes();
            savedTime.stopKey = stopKey;
            savedTime.routeKey = routeKey;

            waitTimesDict[waitTimesIndex] = savedTime;
            waitTimesIndex++;

            int[] minutes = new int[MaxPassages];
            for (int i = 0; i < MaxPassages; i++)
            {
                minutes[i] = NoBusSentinel;
            }

            while (!waitTimesReady) ;

            if (stopsDict.TryGetValue(stopKey, out var stopObj))
            {
                List<Departure> next;
                lock (stopObj.depLock)                           // snapshot under the lock...
                {
                    next = stopObj.stopDepartures
                        .Where(d => d.routeID == routeKey
                                 && d.time >= DateTime.Now)
                        .OrderBy(d => d.time)
                        .Take(MaxPassages)
                        .ToList();                               // ...materialize before releasing
                }                                                // format OUTSIDE the lock

                DateTimeOffset local = TimeZoneInfo.ConvertTime(DateTimeOffset.Now, TimeZoneInfo.Local);
                int hh = local.Hour;
                int mm = local.Minute;
                if (hh == 0)
                {
                    hh = 24;
                }
                for (int i = 0; i < next.Count; i++)
                {
                    int m = (int)Math.Floor((next[i].time - DateTime.Now).TotalMinutes);
                    if (m > 99) m = hh * 100 + mm;
                    if (m < 0) m = 0;                          // clamp tiny clock-skew negatives
                    if (m > 9998) m = 9998;                    // keep 9999 reserved as the sentinel
                    minutes[i] = m;
                }
            }

            //line fixing when number < 99
            if(line.Length < 3)
            {
                int line_int = int.Parse(line);
                line = line_int.ToString("D3");
            }

            //Message 1
            string message = "(" + TIME_REMAINING1_PREFIX + "_" + stop.ToString() + line.ToString() 
                                                          + "_" + minutes[0].ToString("D4")
                                                          + "_" + minutes[1].ToString("D4") + ")";
            int length = message.Length;
            send_frame(message, length, true, NORMAL_PADDING);
            Thread.Sleep(10);

            //Message 2
            message = "(" + TIME_REMAINING2_PREFIX + "_" + stop.ToString() + line.ToString() + "_" + minutes[2].ToString("D4") + ")";
            length = message.Length;
            send_frame(message, length, true, NORMAL_PADDING);

            refresh_tick = true;
        }

        private void button_DoneSending_Click(object sender, EventArgs e)
        {

        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            UART_Buffer_In += serialPort1.ReadExisting();
            rtf_UART.Invoke((Action)(() =>
            {
                rtf_UART.AppendText(UART_Buffer_In + "\n");
                rtf_UART.ScrollToCaret();
            }));

            if (UART_Buffer_In.Contains("(") && UART_Buffer_In.Contains(")")) //Complete message
            {
                incoming_stop_or_line_str = FrameRouter.GetStopLine(UART_Buffer_In);
                switch (FrameRouter.GetAction(UART_Buffer_In))
                {
                    //Stop side
                    case CHECK_STOP:
                        if (stopsDict.ContainsKey("1-" + incoming_stop_or_line_str.TrimStart('0')))
                        {
                            handle_StopValidity(incoming_stop_or_line_str, "G");
                        }
                        else
                        {
                            handle_StopValidity(incoming_stop_or_line_str, "B");
                        }
                        break;
                    case GET_LINES:
                        handle_NbrLines(incoming_stop_or_line_str);
                        break;
                    case ACK_LINE_QUANTITY:
                        handle_LineID();
                        break;

                    //Line side
                    case CHECK_LINE:
                        switch(int.Parse(incoming_stop_or_line_str))
                        {
                            //Lines in the API not in RTCNomade for some reason
                            case 14:
                            case 15:
                            case 25:
                            case 75:
                            case 77:
                            case 315:
                            case 331:
                            case 332:
                            case 344:
                            case 377:
                            case 391:
                            case 577:
                            case 400:
                            case 402:
                            case 405:
                            case 410:
                            case 420:
                            case 915:
                                handle_LineValidity(incoming_stop_or_line_str, "B");
                                break;
                            default:
                                if (routesDict.ContainsKey("1-" + incoming_stop_or_line_str.TrimStart('0')))
                                {
                                    handle_LineValidity(incoming_stop_or_line_str, "G");
                                }
                                else
                                {
                                    handle_LineValidity(incoming_stop_or_line_str, "B");
                                }
                                break;
                        }
                        break;
                        
                    case GET_DIRECTIONS:
                        handle_directions(incoming_stop_or_line_str.TrimStart('0'));
                        break;
                    case GET_STOPS:
                        handle_NbrStops(incoming_stop_or_line_str);
                        break;
                    case ACK_STOP_QUANTITY:
                        handle_StopID();
                        break;
                    case GET_TIMES:
                        refresh_tick = false;
                        int first_underscore = UART_Buffer_In.IndexOf('_');
                        int second_underscore = UART_Buffer_In.LastIndexOf("_");
                        string stop = UART_Buffer_In.Substring(first_underscore + 1, 4);
                        string line = UART_Buffer_In.Substring(second_underscore + 1, 3);
                        handle_TimeRemaining(stop, line.TrimStart('0'));
                        break;
                    case RESET:
                        refresh_tick = false;
                        waitTimesIndex = 0;
                        waitTimesDict.Clear();
                        break;
                }
                incoming_stop_or_line_str = "";
                UART_Buffer_In = "";
            }
        }

        private void button_saveToCache_Click(object sender, EventArgs e)
        {
            SaveCacheToDisk();
        }

        private void button_reloadFromCache_Click(object sender, EventArgs e)
        {
            TryLoadCacheFromDisk();
        }

        private void timerPassageRefresh_Tick(object sender, EventArgs e)
        {
            if (refresh_tick == false)
            {
                return;
            }
            foreach (KeyValuePair<int, RequestedWaitTimes> kvp in waitTimesDict)
            {
                if (stopsDict.TryGetValue(kvp.Value.stopKey, out var stopObj))
                {
                    List<Departure> next;
                    lock (stopObj.depLock)                           // snapshot under the lock...
                    {
                        next = stopObj.stopDepartures
                            .Where(d => d.routeID == kvp.Value.routeKey
                                     && d.time >= DateTime.Now)
                            .OrderBy(d => d.time)
                            .Take(MaxPassages)
                            .ToList();                               // ...materialize before releasing
                    }                                                // format OUTSIDE the lock

                    int[] minutes = new int[MaxPassages];
                    for (int i = 0; i < MaxPassages; i++)
                    {
                        minutes[i] = NoBusSentinel;
                    }

                    DateTimeOffset local = TimeZoneInfo.ConvertTime(DateTimeOffset.Now, TimeZoneInfo.Local);
                    int hh = local.Hour;
                    int mm = local.Minute;
                    if (hh == 0)
                    {
                        hh = 24;
                    }
                    for (int i = 0; i < next.Count; i++)
                    {
                        int m = (int)Math.Floor((next[i].time - DateTime.Now).TotalMinutes);
                        if (m > 99) m = hh * 100 + mm;
                        if (m < 0) m = 0;                          // clamp tiny clock-skew negatives
                        if (m > 9998) m = 9998;                    // keep 9999 reserved as the sentinel
                        minutes[i] = m;
                    }

                    string line_leading0 = "";
                    //line fixing when number < 99
                    if (kvp.Value.routeKey.Substring(2).Length < 3)
                    {
                        int line_int = int.Parse(kvp.Value.routeKey.Substring(2));
                        line_leading0 = line_int.ToString("D3");
                    }
                    else
                    {
                        line_leading0 = kvp.Value.routeKey.Substring(2);
                    }

                    //Message 1
                    string message = "(" + TIME_REMAINING1_PREFIX + "_" + kvp.Value.stopKey.Substring(2) + line_leading0
                                                                      + "_" + minutes[0].ToString("D4")
                                                                      + "_" + minutes[1].ToString("D4") + ")";
                    int length = message.Length;
                    send_frame(message, length, true, NORMAL_PADDING);
                    Thread.Sleep(10);

                    //Message 2
                    message = "(" + TIME_REMAINING2_PREFIX + "_" + kvp.Value.stopKey.Substring(2) + line_leading0 + "_" + minutes[2].ToString("D4") + ")";
                    length = message.Length;
                    send_frame(message, length, true, NORMAL_PADDING);
                }
            }
        }
    }
}