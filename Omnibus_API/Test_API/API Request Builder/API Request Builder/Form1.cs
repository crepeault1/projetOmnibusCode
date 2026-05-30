using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design.Serialization;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO.Ports;
using System.Linq;
//Includes
using System.Net.Http;
using System.Net.Http.Headers;
using System.Security.Permissions;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Web;
using System.Windows.Forms;
using static API_Request_Builder.Form1;
using static System.DateTimeOffset;
using static System.Net.WebRequestMethods;
using System.Threading;
using System.IO;

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
        const string STOP_IDENTIFIER_PREFIX = "LIDN";
        const string DONE_PREFIX = "DONE";
        const string TIME_REMAINING_PREFIX = "REMN";

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


        string UART_Buffer_In = "";
        string UART_Buffer_Out = "";

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

        public class Stop
        {
            public string stopTransitlandID { get; set; }
            public string stopID { get; set; }
            public string stopName { get; set; }
            bool isStopSelected;
            public List<Route> routesAtThisStop { get; set; } = new List<Route>();
            public List<DateTimeOffset> stopDepartures { get; set; } = new List<DateTimeOffset>();
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

        Dictionary<string, Stop> stopsDict = new Dictionary<string, Stop>();
        Dictionary<string, Route> routesDict = new Dictionary<string, Route>();

        HttpClient client = new HttpClient();

        int incoming_stop_or_line_int;
        string incoming_stop_or_line_str;
        string saved_stop_or_line_str;
        public struct Frame
        {
            public string header;
            public int action;
        }

        public class FrameRouter
        {
            int incoming_stop_or_line;
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
                new Frame { header = "(WAIT", action = GET_TIMES}
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
            
            DateTimeOffset rightNow = DateTimeOffset.Now;
            labelCurrentTime.Text = rightNow.ToString();
            if (rtfBoxAllDepartures.Text != "")
            {
                int counter = 0;
                foreach (DateTimeOffset storedDeparture in stopsDict[selectedStop].stopDepartures)
                {
                    if (DateTimeOffset.Compare(storedDeparture, rightNow) > 0 && counter < 3) //Departure is after right now
                    {
                        System.TimeSpan timeToNextPassage = storedDeparture.Subtract(rightNow);
                        nextThreeDepartures[counter] = timeToNextPassage.ToString("mm");
                        counter++;
                    }
                }
                textBoxNextDeparture1.Text = nextThreeDepartures[0];
                textBoxNextDeparture2.Text = nextThreeDepartures[1];
                textBoxNextDeparture3.Text = nextThreeDepartures[2];

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
                    routes = s.routesAtThisStop.Select(r => r.routeID).ToList()
                }).ToList()
            };

            string json = JsonSerializer.Serialize(cacheObject,
                new JsonSerializerOptions { WriteIndented = true });
            System.IO.File.WriteAllText(cacheFilePath, json);
            labelStatus.Text = "Cache saved to disk";
        }

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

        private async void buttonDeparturesQuery_Click(object sender, EventArgs e)
        {
            rtfBox_json.Clear();
            labelStatusDepartures.Text = "Request Ongoing";
            selectedRoute = textBoxSelectedRoute.Text; // Hook here when calling from pico
            selectedStop = textBoxSelectedStop.Text;
            string nextURL = textBoxDeparturesQuery.Text;
            while (nextURL != null)
            {
                var response = await client.GetAsync(nextURL);
                response.EnsureSuccessStatusCode();
                labelStatusDepartures.Text = "API Reply";

                var json = await response.Content.ReadAsStringAsync();
                JsonDocument doc = JsonDocument.Parse(json);
                JsonElement root = doc.RootElement;
                JsonElement departureArray = root.GetProperty("stops").EnumerateArray().First();
                labelStatusDepartures.Text = "Parsing...";

                foreach (JsonElement departures in departureArray.GetProperty("departures").EnumerateArray())
                {
                    if ((departures.GetProperty("trip").
                                    GetProperty("route").
                                    GetProperty("route_id").
                                    GetString()) == selectedRoute)
                    {

                        string stopDepartureTime = departures.GetProperty("departure").GetProperty("scheduled_utc").GetString();

                        DateTimeOffset utc = DateTimeOffset.Parse(stopDepartureTime);
                        TimeZoneInfo localZone = TimeZoneInfo.FindSystemTimeZoneById("Eastern Standard Time");
                        DateTimeOffset local = TimeZoneInfo.ConvertTime(utc, localZone);

                        string displayTime = local.ToString("HH:mm");
                        rtfBoxAllDepartures.Text += displayTime + ' ';

                        stopsDict[selectedStop].stopDepartures.Add(local);

                    }
                }

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
            }
            labelStatusDepartures.Text = "Request Completed";
        }

        private void comboBoxAvailableRoutes_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxAvailableRoutes.SelectedItem == null) return;
            handle_AvailableRoutes(comboBoxAvailableRoutes.SelectedItem.ToString());
        }

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

        private void comboBoxAvailableStops_SelectedIndexChanged(object sender, EventArgs e)
        {
            string currentStop = comboBoxAvailableStops.SelectedItem.ToString();

            filterRoutesComboBox(currentStop);
        }

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

        private void buttonConfirmSelection_Click(object sender, EventArgs e)
        {
            if ((comboBoxAvailableRoutes.Text != "") && (comboBoxAvailableStops.Text != ""))
            {
                string selectedStop = comboBoxAvailableStops.SelectedItem.ToString();
                string stopID = selectedStop.Split(' ')[0];
                string desiredTransitLandID = stopsDict[stopID].stopTransitlandID;

                textBoxDeparturesQuery.Text = departuresQuery1 +
                    desiredTransitLandID +
                    departuresQuery2 +
                    start_time +
                    end_time +
                    resultsNumber +
                    relative_date;
                textBoxSelectedRoute.Text = comboBoxAvailableRoutes.Text;
                textBoxSelectedStop.Text = stopID;
            }
        }

        private void handle_API_URL(string test)
        {

        }

        private void populateRoutesComboBox()
        {
            comboBoxAvailableRoutes.Items.Clear(); //duplicate protection
            foreach (KeyValuePair<string, Route> kvp in routesDict)
            {
                comboBoxAvailableRoutes.Items.Add(kvp.Key);
            }
        }

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

        private async Task LoadDirectionsForRouteAsync(Route route)
        {
            if (route.directionsLoaded) return;

            string nextURL = tripsQueryPrefix + route.routeOnestopID
                            + tripsQuerySuffix + tripsRelativeDate;

            // Dedupe by (direction_id, headsign) — same destination = same UI entry.
            // The Dictionary value lets us look up the existing Direction so we can
            // append extra trip IDs to it for the union-of-stops merge later.
            var seenDirections = new Dictionary<string, Direction>();

            labelStatus.Text = "Loading directions...";

            while (nextURL != null)
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
                        // Same destination, different pattern → it's a variant
                        // (short turn, express, etc). Remember its trip ID so
                        // LoadStopsForDirectionAsync can later union in any extra stops.
                        // Avoid re-adding the same pattern twice in case the feed
                        // happens to repeat (it shouldn't, but cheap to guard).
                        if (existing.stopPatternId != patternId
                            && (existing.additionalTripIDs == null
                                || !existing.additionalTripIDs.Contains(tripIntID)))
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

            route.directionsLoaded = true;
            labelStatus.Text = "Directions loaded";
        }

        private async Task LoadStopsForDirectionAsync(Route route, Direction direction)
        {
            if (direction.stopsLoaded) return;

            // --- Step 1: Load the canonical (representative) trip's stops ---

            string url = tripsQueryPrefix + route.routeOnestopID
                         + "/trips/" + direction.representativeTripID;

            labelStatus.Text = "Loading stops for direction...";

            var response = await client.GetAsync(url);
            response.EnsureSuccessStatusCode();

            var json = await response.Content.ReadAsStringAsync();
            JsonDocument doc = JsonDocument.Parse(json);
            JsonElement root = doc.RootElement;

            JsonElement tripsArray = root.GetProperty("trips");
            JsonElement trip = tripsArray.EnumerateArray().First();

            if (!trip.TryGetProperty("stop_times", out JsonElement stopTimes))
            {
                labelStatus.Text = "No stop_times in trip response";
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
            labelStatus.Text = "Stops loaded";
        }

        private async void comboBoxAvailableDirections_SelectedIndexChanged(object sender, EventArgs e)
        {
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
        }

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


        private void send_frame(string message, int length, bool output)
        {
            if (message.Length > 24)
            {
                message = message.Substring(0, 23);
                message += ")";
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

        private void button_Epoch_Click(object sender, EventArgs e)
        {
            TimeSpan t = DateTime.UtcNow - new DateTime(1970, 1, 1);
            int secondsSinceEpoch = (int)t.TotalSeconds;
            handle_epoch(secondsSinceEpoch);
        }

        private void handle_epoch(long epoch_val)
        {
            string message = "(" + EPOCH_PREFIX + "_" + epoch_val.ToString() + ")";
            int length = message.Length;
            send_frame(message, length, false);
        }

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
            send_frame(message, length, true);
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
            send_frame(message, length, true);
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
                send_frame(message, length, true);
                Thread.Sleep(10);
            }
            saved_stop_or_line_str = "";
        }
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
            send_frame(message, length, true);
        }

        private void button_Directions_Click(object sender, EventArgs e)
        {

        }

        private void handle_directions(string line)
        {
            saved_stop_or_line_str = line;
            foreach (Direction direction in routesDict["1-" + line].directionsOnThisRoute)
            {
                string message = "(" + DIRECTIONS_PREFIX + "_" + direction.headsign + ")";
                int length = message.Length;
                send_frame(message, length, true);
                Thread.Sleep(10);
            }
        }

        private void handle_userDirection(string choice)
        {
            int choice_int = int.Parse(choice);
            var direction = routesDict["1-" + saved_stop_or_line_str].directionsOnThisRoute.FirstOrDefault(d => d.directionId == choice_int);
            if (choice.Contains('0'))
            {
                foreach(Stop stop in direction.stopsInThisDirection)
                {

                }
            }
            else if(choice.Contains('1'))
            {

            }
        }
        private void button_NbrStops_Click(object sender, EventArgs e)
        {

        }

        private void button_StopID_Click(object sender, EventArgs e)
        {

        }

        private void button_TimeRemaining_Click(object sender, EventArgs e)
        {

        }

        private void handle_TimeRemaining(string stop, string line)
        {

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
            }));

            if (UART_Buffer_In.Contains("(") && UART_Buffer_In.Contains(")")) //Complete message
            {
                incoming_stop_or_line_str = FrameRouter.GetStopLine(UART_Buffer_In);
                incoming_stop_or_line_int = int.Parse(incoming_stop_or_line_str);
                switch (FrameRouter.GetAction(UART_Buffer_In))
                {
                    //Stop side
                    case CHECK_STOP:
                        if (stopsDict.ContainsKey("1-" + incoming_stop_or_line_str))
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
                        if (routesDict.ContainsKey("1-" + incoming_stop_or_line_str))
                        {
                            handle_LineValidity(incoming_stop_or_line_str, "G");
                        }
                        else
                        {
                            handle_LineValidity(incoming_stop_or_line_str, "B");
                        }
                        break;
                    case GET_DIRECTIONS:
                        handle_directions(incoming_stop_or_line_str);
                        break;
                    case SET_DIRECTION:
                        handle_userDirection(incoming_stop_or_line_str);
                        break;
                    case GET_STOPS:
                        break;
                    case ACK_STOP_QUANTITY:
                        break;


                    case GET_TIMES:
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
    }
}