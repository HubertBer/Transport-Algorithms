#!/usr/bin/env python3

import argparse
import os

import osmnx as ox
import pandas as pd

DEFAULT_PLACE = "Kraków, Poland"
DEFAULT_OUTPUT = "data/maps/krakow"
ROAD_TYPE = "primary|primary_link"


def download_and_export(place: str, output_dir: str) -> None:
    os.makedirs(output_dir, exist_ok=True)

    print(f"[1/4] Querying OSM for roads in: {place}")
    G = ox.graph_from_place(place, custom_filter=f'["highway"~"{ROAD_TYPE}"]')

    print("[2/4] Converting to undirected graph …")
    G = ox.convert.to_undirected(G)

    print("[3/4] Extracting node / edge GeoDataFrames …")
    gdf_nodes, gdf_edges = ox.graph_to_gdfs(G)

    osmid_to_id: dict[int, int] = {
        osmid: idx for idx, osmid in enumerate(gdf_nodes.index)
    }

    nodes_df = pd.DataFrame(
        {
            "id": [osmid_to_id[osmid] for osmid in gdf_nodes.index],
            "osmid": list(gdf_nodes.index),
            "lat": gdf_nodes["y"].to_numpy(),
            "lon": gdf_nodes["x"].to_numpy(),
        }
    )

    nodes_path = os.path.join(output_dir, "nodes.csv")
    nodes_df.to_csv(nodes_path, index=False)
    print(f"         {len(nodes_df):>6} nodes  →  {nodes_path}")

    u_nodes = [u for u, v, _ in gdf_edges.index]
    v_nodes = [v for u, v, _ in gdf_edges.index]

    edges_df = pd.DataFrame(
        {
            "from_id": [osmid_to_id[u] for u in u_nodes],
            "to_id": [osmid_to_id[v] for v in v_nodes],
            "distance_m": gdf_edges["length"].to_numpy(),
        }
    )

    edges_path = os.path.join(output_dir, "edges.csv")
    edges_df.to_csv(edges_path, index=False)
    print(f"         {len(edges_df):>6} edges  →  {edges_path}")

    print("[4/4] Done.")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Download an OSM road network and export it as CSV files.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--place", default=DEFAULT_PLACE)
    parser.add_argument("--output", default=DEFAULT_OUTPUT)
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    download_and_export(args.place, args.output)
